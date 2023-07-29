// Fill out your copyright notice in the Description page of Project Settings.
#include "FunKTestRunner.h"
#include "FileHelpers.h"
#include "FunK.h"
#include "FunKEngineSubsystem.h"
#include "Captures/FunKFailedPieStartCapture.h"
#include "Captures/FunKNewProcessCapture.h"
#include "FunKSettingsObject.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Editor/EditorPerformanceSettings.h"
#include "Environment/FunKEditorEnvironmentHandler.h"
#include "Events/FunKEvent.h"
#include "GameFramework/GameStateBase.h"
#include "Misc/AutomationTest.h"
#include "UObject/UnrealTypePrivate.h"

class UFunKSettingsObject;

void UFunKTestRunner::Init(UFunKEngineSubsystem* funKEngineSubsystem, EFunKTestRunnerType RunType)
{
	Type = RunType;
	FunKEngineSubsystem = funKEngineSubsystem;
	EnvironmentHandler = RunType == EFunKTestRunnerType::LocalInProc
		? NewObject<UFunKEditorEnvironmentHandler>()
		: nullptr;

	UpdateState(EFunKTestRunnerState::Initialized);
}

bool UFunKTestRunner::Start(const FFunKTestInstructions& Instructions)
{	
	UpdateState(Type > EFunKTestRunnerType::LocalInProc ? EFunKTestRunnerState::ExecutingTest : EFunKTestRunnerState::Started);
	RaiseInfoEvent("FunK Start");
	ActiveTestInstructions = Instructions;	
	return true;
}

bool UFunKTestRunner::Update()
{
	if(Type != EFunKTestRunnerType::LocalInProc)
	{
		RaiseErrorEvent("The tests can currently only be started via. Editor!", "UFunKTestRun::Next");
		return true;
	}
	
	if(State != EFunKTestRunnerState::ExecutingTest)
	{
		if(State <= EFunKTestRunnerState::Ended)
		{
			RaiseErrorEvent("The test run is not active!", "UFunKTestRun::Next");
			return true;
		}

		EFunKEnvironmentWorldState environmentState = EnvironmentHandler->UpdateWorldState(ActiveTestInstructions);
		if(environmentState == EFunKEnvironmentWorldState::CantStart)
		{
			RaiseErrorEvent("Test environment could not be setup", "UFunKTestRunner::StartEnvironment");
			End();
			return true;
		}
		
		UpdateState(environmentState == EFunKEnvironmentWorldState::IsRunning ? EFunKTestRunnerState::Ready : EFunKTestRunnerState::EnvironmentSetup);

		if(State == EFunKTestRunnerState::Started)
		{
			bool isWrongEnvironment = false;
			const bool isEnvironmentRunning = IsEnvironmentRunning(Instructions, isWrongEnvironment);
			if(!isEnvironmentRunning || isWrongEnvironment)
			{
				if(isWrongEnvironment)
				{
					if(isEnvironmentRunning)
						GEditor->EndPlayMap();
				
					GEditor->EndPlayOnLocalPc();
					StartedProcesses.Empty(StartedProcesses.Num());
				}
			
				const EFunKTestRunnerState state = State;
				UpdateState(EFunKTestRunnerState::WaitingForWorld);
				if(!StartEnvironment(Instructions))
				{
					UpdateState(state);
					RaiseErrorEvent("Test environment could not be setup", "UFunKTestRunner::StartEnvironment");
					return true;
				}

				ActiveTestInstructions = Instructions;
			}
			else
			{
				UpdateState(EFunKTestRunnerState::WaitingForWorld);
				ActiveTestInstructions = Instructions;
			}
		}
		
		if(State == EFunKTestRunnerState::WaitingForWorld)
		{
			if (CurrentTestWorld.IsValid() && CurrentTestWorld->AreActorsInitialized() )
			{
				const AGameStateBase* GameState = CurrentTestWorld->GetGameState();
				if (GameState && GameState->HasMatchStarted())
				{
					UpdateState(IsStandaloneTest() ? EFunKTestRunnerState::Ready : EFunKTestRunnerState::WaitingForConnections);
				}
			}
		}

		if (State == EFunKTestRunnerState::WaitingForConnections)
		{
			if(AFunKWorldTestController* worldController = GetCurrentWorldController())
			{
				if(worldController->GetActiveControllerCount() >= 3)
				{
					UpdateState(EFunKTestRunnerState::Ready);
				}
			}
		}
		
		if(State == EFunKTestRunnerState::Ready)
		{
			EventBusRegistration.Add(EnvironmentHandler->GetEventBus()->On<FFunKEvent>([this](const FFunKEvent& Event)
			{
				RaiseEvent(Event);
			}));
			
			if(ActiveTestInstructions.MapTestName.Len() > 0)
			{
				GetCurrentWorldController()->ExecuteTestByName(ActiveTestInstructions.MapTestName, this);
			}
			else
			{
				GetCurrentWorldController()->ExecuteAllTests(this);
			}
			
			UpdateState(EFunKTestRunnerState::ExecutingTest);
		}
	}
	else
	{
		const AFunKWorldTestController* worldController = GetCurrentWorldController();
		if(!worldController)
		{
			RaiseWarningEvent("Canceled test run: world is tearing down!");
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
		else if(worldController->IsFinished())
		{
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
	}

	if(State == EFunKTestRunnerState::EvaluatingTest)
	{
		End();
		return true;
	}

	return false;
}

void UFunKTestRunner::End()
{
	UpdateState(EFunKTestRunnerState::Ended);
	EventBusRegistration.Unregister();
}

void UFunKTestRunner::RaiseInfoEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Info(Message, Context));
}

void UFunKTestRunner::RaiseWarningEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Warning(Message, Context));
}

void UFunKTestRunner::RaiseErrorEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Error(Message, Context));
}

void UFunKTestRunner::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(auto* AutomationEntryRuntime = FAutomationTestFramework::Get().GetCurrentTest())
	{	
		EAutomationEventType type = raisedEvent.Type == EFunKEventType::Info
			? EAutomationEventType::Info
			: raisedEvent.Type == EFunKEventType::Warning
				? EAutomationEventType::Warning
				: EAutomationEventType::Error;
		
		AutomationEntryRuntime->AddEvent(FAutomationEvent(type, raisedEvent.Message, raisedEvent.GetContext()));
	}
}

bool UFunKTestRunner::IsRunning() const
{
	return State >= EFunKTestRunnerState::Started;
}

bool UFunKTestRunner::IsWaitingForMap() const
{
	return State == EFunKTestRunnerState::WaitingForWorld;
}

UFunKEngineSubsystem* UFunKTestRunner::GetSubsystem() const
{
	return FunKEngineSubsystem.Get();
}

const FString& UFunKTestRunner::GetParameter()
{
	return ActiveTestInstructions.Params;
}

bool UFunKTestRunner::SetWorld(UWorld* world)
{
	if(!world)
		return false;
	
	if(State != EFunKTestRunnerState::WaitingForWorld && Type <= EFunKTestRunnerType::LocalInProc)
		return false;

	if(world == CurrentTestWorld)
		return false;
	
	CurrentTestWorld = world;	
	return true;
}

AFunKWorldTestController* UFunKTestRunner::GetCurrentWorldController() const
{
	if(!CurrentTestWorld.IsValid())
		return nullptr;
	
	if(CurrentTestWorld->bIsTearingDown)
		return nullptr;
		
	return CurrentTestWorld->GetSubsystem<UFunKWorldSubsystem>()->GetLocalTestController();
}

void UFunKTestRunner::GetSinks(TArray<TScriptInterface<IFunKSink>>& outSinks)
{
	outSinks.Add(NewObject<UFunKLogSink>(this, UFunKLogSink::StaticClass()));
	if(UFunKAutomationSink::IsAvailable())
	{
		outSinks.Add(NewObject<UFunKAutomationSink>(this, UFunKAutomationSink::StaticClass()));
	}
}

void UFunKTestRunner::UpdateState(EFunKTestRunnerState newState)
{
	State = newState;
}

bool UFunKTestRunner::IsStandaloneTest() const
{
	return IsStandaloneTest(ActiveTestInstructions);
}

bool UFunKTestRunner::IsStandaloneTest(const FFunKTestInstructions& Instructions)
{
	return Instructions.Params.Contains(FFunKModule::FunkStandaloneParameter);
}

bool UFunKTestRunner::IsDedicatedServerTest() const
{
	return IsDedicatedServerTest(ActiveTestInstructions);
}

bool UFunKTestRunner::IsDedicatedServerTest(const FFunKTestInstructions& Instructions)
{
	return Instructions.Params.Contains(FFunKModule::FunkDedicatedParameter);
}

bool UFunKTestRunner::IsListenServerTest() const
{
	return IsListenServerTest(ActiveTestInstructions);
}

bool UFunKTestRunner::IsListenServerTest(const FFunKTestInstructions& Instructions)
{
	return Instructions.Params.Contains(FFunKModule::FunkListenParameter);
}

bool UFunKTestRunner::IsDifferentEnvironment(const FFunKTestInstructions& Instructions) const
{
	return Instructions.MapObjectPath != ActiveTestInstructions.MapObjectPath || Instructions.MapPackageName != ActiveTestInstructions.MapPackageName || Instructions.Params != ActiveTestInstructions.Params;
}

bool UFunKTestRunner::IsEnvironmentRunning(const FFunKTestInstructions& Instructions, bool& isWrongEnvironmentRunning)
{
	isWrongEnvironmentRunning = false;

	const FFunKSettings& settings = GetDefault<UFunKSettingsObject>()->Settings;
	const FString currentPieWorldPackageName = GetCurrentPieWorldPackageName();
	const bool isNoLocalMapRunning = (currentPieWorldPackageName.IsEmpty() || currentPieWorldPackageName.Len() <= 0);
	const bool isLocalProcessOnly = settings.RunTestUnderOneProcess || IsStandaloneTest(Instructions);
	if(isNoLocalMapRunning)
	{
		isWrongEnvironmentRunning = !isLocalProcessOnly && IsHoldingSubprocesses();
		return false;
	}

	isWrongEnvironmentRunning = currentPieWorldPackageName != Instructions.MapPackageName || IsDifferentEnvironment(Instructions);
	if(isWrongEnvironmentRunning || isLocalProcessOnly)
		return true;

	for (const uint32 StartedProcess : StartedProcesses)
	{
		if(!FPlatformProcess::IsApplicationRunning(StartedProcess))
		{
			isWrongEnvironmentRunning = true;
			break;
		}
	}

	return true;
}

FString UFunKTestRunner::GetCurrentPieWorldPackageName()
{
	//TODO: do we want to support multiple worlds in parallel?
	const TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();
	for (auto& Context : WorldContexts)
	{
		if (Context.World())
		{
			FString WorldPackage = Context.World()->GetOutermost()->GetName();

			if (Context.WorldType == EWorldType::PIE)
			{
				return UWorld::StripPIEPrefixFromPackageName(WorldPackage, Context.World()->StreamingLevelsPrefix);
			}
		}
	}

	return "";
}

bool UFunKTestRunner::IsHoldingSubprocesses() const
{
	return StartedProcesses.Num() > 0;
}

bool UFunKTestRunner::StartEnvironment(const FFunKTestInstructions& Instructions)
{
	FFunKFailedPieStartCapture FailHandler;
	FFunKNewProcessCapture ProcessStartCapture;

	UEditorPerformanceSettings* Settings = GetMutableDefault<UEditorPerformanceSettings>();
	Settings->bThrottleCPUWhenNotForeground = false;
	Settings->bMonitorEditorPerformance = false;
	Settings->PostEditChange();
		
	FRequestPlaySessionParams params;
	params.EditorPlaySettings = NewObject<ULevelEditorPlaySettings>();
	params.EditorPlaySettings->NewWindowHeight = 1080;
	params.EditorPlaySettings->NewWindowWidth = 1920;
	
	params.GlobalMapOverride = Instructions.MapPackageName;
	params.AdditionalStandaloneCommandLineParameters = FFunKModule::FunkTestStartParameter;
	
	// GEditor->EndPlayMap();

	FProperty* Property = ULevelEditorPlaySettings::StaticClass()->FindPropertyByName(FName("ServerMapNameOverride"));
	if(Property)
	{
		Property->SetValue_InContainer(params.EditorPlaySettings, &Instructions.MapPackageName);
	}
	
	//TODO: Maybe one day we want to integrate bAllowOnlineSubsystem...
	const FFunKSettings& settings = GetDefault<UFunKSettingsObject>()->Settings;

	const bool isStandalone = IsStandaloneTest(Instructions);
	if(isStandalone)
	{
		params.EditorPlaySettings->SetPlayNumberOfClients(1);
		params.EditorPlaySettings->bLaunchSeparateServer = false;
		params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
		SetFpsSettings(params.EditorPlaySettings, settings, false);
		
		GEditor->RequestPlaySession(params);
	}
	else
	{
		if(IsListenServerTest(Instructions))
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(3);
			params.EditorPlaySettings->bLaunchSeparateServer = false;
			params.EditorPlaySettings->SetRunUnderOneProcess(settings.RunTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_ListenServer);
			SetFpsSettings(params.EditorPlaySettings, settings, false);
			
			GEditor->RequestPlaySession(params);
		}
		else if(IsDedicatedServerTest(Instructions))
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(2);
			params.EditorPlaySettings->bLaunchSeparateServer = true;
			params.EditorPlaySettings->SetRunUnderOneProcess(settings.RunTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Client);
			SetFpsSettings(params.EditorPlaySettings, settings, true);
			
			GEditor->RequestPlaySession(params);
		}
		else
		{
			RaiseErrorEvent("Invalid Parameter at start");
			return false;
		}
	}

	// Immediately launch the session 
	GEditor->StartQueuedPlaySessionRequest();
	if(!isStandalone && !settings.RunTestUnderOneProcess)
		ProcessStartCapture.GetProcessIds(StartedProcesses);
	
	return FailHandler.CanProceed() && (settings.RunTestUnderOneProcess || isStandalone || (ProcessStartCapture.IsValid() && StartedProcesses.Num() == 2));
}

void UFunKTestRunner::SetFpsSettings(ULevelEditorPlaySettings* playSettings, const FFunKSettings& funkSettings, bool isDedicated)
{
	if(funkSettings.FixedTickFrameRates.Num() <= 0)
		return;

	const int32 server = funkSettings.FixedTickFrameRates[0];
	if(isDedicated)
		playSettings->ServerFixedFPS = server;
	else
		playSettings->ClientFixedFPS.Add(server);

	int32 client1 = server;
	int32 client2 = server;
	if(funkSettings.FixedTickFrameRates.Num() == 2)
	{
		client1 = client2 = funkSettings.FixedTickFrameRates[1];
	}
	else if(funkSettings.FixedTickFrameRates.Num() > 2)
	{
		client1 = funkSettings.FixedTickFrameRates[1];
		client2 = funkSettings.FixedTickFrameRates[2];
	}

	playSettings->ClientFixedFPS.Add(client1);
	playSettings->ClientFixedFPS.Add(client2);
}

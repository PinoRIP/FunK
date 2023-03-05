// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestRunner.h"

#include "FileHelpers.h"
#include "FunK.h"
#include "FunKAutomationEntry.h"
#include "FunKEngineSubsystem.h"
#include "FunKWorldTestController.h"
#include "GameFramework/GameStateBase.h"
#include "Sinks/FunKExtAutomationSink.h"
#include "Sinks/FunKInProcAutomationSink.h"
#include "Sinks/FunKLogSink.h"
#include "Sinks/FunKSink.h"
#include "UObject/UnrealTypePrivate.h"

void UFunKTestRunner::Init(UFunKEngineSubsystem* funKEngineSubsystem, EFunKTestRunnerType RunType)
{
	Type = RunType;
	FunKEngineSubsystem = funKEngineSubsystem;

	TArray<TSubclassOf<UFunKSink>> sinkTypes;
	GetSinks(sinkTypes);

	Sinks.Reserve(sinkTypes.Num());
	for (const TSubclassOf<UFunKSink>& sinkType : sinkTypes)
	{
		Sinks.Add(NewSink(sinkType));
	}

	UpdateState(EFunKTestRunnerState::Initialized);
}

void UFunKTestRunner::Start()
{	
	UpdateState(Type > EFunKTestRunnerType::LocalInProc ? EFunKTestRunnerState::ExecutingTest : EFunKTestRunnerState::Started);
	RaiseStartEvent();
}

bool UFunKTestRunner::Test(const FFunKTestInstructions& Instructions)
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
			RaiseErrorEvent("The test run ist not active!", "UFunKTestRun::Next");
			return true;
		}

		if(IsDifferentEnvironment(Instructions))
		{
			if(!StartEnvironment(Instructions))
			{
				RaiseErrorEvent("Test environment could not be setup", "UFunKTestRunner::StartEnvironment");
				return true;
			}

			ActiveTestInstructions = Instructions;
			UpdateState(EFunKTestRunnerState::WaitingForWorld);
		}
		
		if(State == EFunKTestRunnerState::WaitingForWorld)
		{
			if (CurrentTestWorld && CurrentTestWorld->AreActorsInitialized() )
			{
				AGameStateBase* GameState = CurrentTestWorld->GetGameState();
				if (GameState && GameState->HasMatchStarted())
				{
					UpdateState(IsStandaloneTest() ? EFunKTestRunnerState::Ready : EFunKTestRunnerState::WaitingForConnections);
				}
			}
		}

		if (State == EFunKTestRunnerState::WaitingForConnections)
		{
			if(CurrentWorldController)
			{
				int32 players = IsDedicatedServerTest() ? 2 : 3;
				if(CurrentTestWorld->GetGameState()->PlayerArray.Num() >= players)
				{
					UpdateState(EFunKTestRunnerState::Ready);
				}
			}
		}

		if(State == EFunKTestRunnerState::Ready)
		{
			if(ActiveTestInstructions.TestName.IsSet())
			{
				CurrentWorldController->ExecuteTestByName(ActiveTestInstructions.TestName.GetValue());
			}
			else
			{
				CurrentWorldController->ExecuteAllTests();
			}
			
			UpdateState(EFunKTestRunnerState::ExecutingTest);
		}
	}
	else
	{
		if(CurrentWorldController->IsFinished())
		{
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
	}

	if(State == EFunKTestRunnerState::EvaluatingTest)
	{
		return true;
	}

	RaiseInfoEvent("WAIT");
	return false;
}

void UFunKTestRunner::End()
{
	UpdateState(EFunKTestRunnerState::Ended);
	CurrentTestWorld = nullptr;
	CurrentWorldController = nullptr;
	
	for (UFunKSink* FunKSink : Sinks)
	{
		if(FunKSink)
		{
			FunKSink->End(this);
		}
	}
	Sinks.Empty();
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
	for (TWeakObjectPtr<UFunKSink> FunKSink : Sinks)
	{
		if(FunKSink.IsValid())
		{
			FunKSink->RaiseEvent(raisedEvent, this);
		}
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

	if(CurrentTestWorld->GetNetMode() != NM_Client)
	{
		CurrentWorldController = CurrentTestWorld->SpawnActor<AFunKWorldTestController>(GetWorldControllerClass());
		CurrentWorldController->SetTestRunner(this);
	}
	
	return true;
}

bool UFunKTestRunner::RegisterWorldController(AFunKWorldTestController* localTestController)
{
	if(CurrentTestWorld->GetNetMode() != NM_Client)
		return false;
	
	CurrentWorldController = localTestController;
	CurrentWorldController->SetTestRunner(this);

	return true;
}

AFunKWorldTestController* UFunKTestRunner::GetCurrentWorldController() const
{
	return CurrentWorldController;
}

void UFunKTestRunner::RaiseStartEvent()
{
	RaiseInfoEvent("FunK Start");
}

void UFunKTestRunner::GetSinks(TArray<TSubclassOf<UFunKSink>>& outSinks)
{
	outSinks.Add(TSubclassOf<UFunKSink>(UFunKLogSink::StaticClass()));
	if(UFunKInProcAutomationSink::IsAvailable())
	{
		outSinks.Add(TSubclassOf<UFunKSink>(UFunKInProcAutomationSink::StaticClass()));
	}
	else
	{
		outSinks.Add(TSubclassOf<UFunKSink>(UFunKExtAutomationSink::StaticClass()));
	}
}

UFunKSink* UFunKTestRunner::NewSink(TSubclassOf<UFunKSink> sinkType)
{
	UFunKSink* sink = NewObject<UFunKSink>(this, sinkType);
	sink->Init(this);
	return sink;
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
	return Instructions.MapTestName != ActiveTestInstructions.MapTestName || Instructions.Params != ActiveTestInstructions.Params;
}

TSubclassOf<AFunKWorldTestController> UFunKTestRunner::GetWorldControllerClass() const
{
	return TSubclassOf<AFunKWorldTestController>(AFunKWorldTestController::StaticClass());
}

bool UFunKTestRunner::StartEnvironment(const FFunKTestInstructions& Instructions)
{
	struct FFailedGameStartHandler
	{
		bool bCanProceed;

		FFailedGameStartHandler()
		{
			bCanProceed = true;
			FEditorDelegates::EndPIE.AddRaw(this, &FFailedGameStartHandler::OnEndPIE);
		}

		~FFailedGameStartHandler()
		{
			FEditorDelegates::EndPIE.RemoveAll(this);
		}

		bool CanProceed() const { return bCanProceed; }

		void OnEndPIE(const bool bInSimulateInEditor)
		{
			bCanProceed = false;
		}
	};

	FFailedGameStartHandler FailHandler;
		
	FRequestPlaySessionParams params;
	params.EditorPlaySettings = NewObject<ULevelEditorPlaySettings>();
	params.EditorPlaySettings->NewWindowHeight = 1080;
	params.EditorPlaySettings->NewWindowWidth = 1920;
	params.GlobalMapOverride = Instructions.MapPackageName;
	params.AdditionalStandaloneCommandLineParameters = FFunKModule::FunkTestStartParameter;

	FProperty* Property = ULevelEditorPlaySettings::StaticClass()->FindPropertyByName(FName("ServerMapNameOverride"));
	if(Property)
	{
		Property->SetValue_InContainer(params.EditorPlaySettings, &Instructions.MapPackageName);
	}
	
	//TODO: Maybe one day we want to integrate bAllowOnlineSubsystem...

	if(IsStandaloneTest(Instructions))
	{
		params.EditorPlaySettings->SetPlayNumberOfClients(1);
		params.EditorPlaySettings->bLaunchSeparateServer = false;
		params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
		
		GEditor->RequestPlaySession(params);
	}
	else
	{
		if(IsListenServerTest(Instructions))
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(3);
			params.EditorPlaySettings->bLaunchSeparateServer = false;
			params.EditorPlaySettings->SetRunUnderOneProcess(IsRunningTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_ListenServer);
			GEditor->RequestPlaySession(params);
		}
		else if(IsDedicatedServerTest(Instructions))
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(2);
			params.EditorPlaySettings->bLaunchSeparateServer = true;
			params.EditorPlaySettings->SetRunUnderOneProcess(IsRunningTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Client);
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
	return FailHandler.CanProceed();
}

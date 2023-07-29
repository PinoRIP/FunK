﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestRunner.h"

#include "FileHelpers.h"
#include "FunK.h"
#include "FunKAutomationEntry.h"
#include "FunKEngineSubsystem.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "GameFramework/GameStateBase.h"
#include "Sinks/FunKAutomationSink.h"
#include "Sinks/FunKLogSink.h"
#include "Sinks/FunKSink.h"
#include "UObject/UnrealTypePrivate.h"

void UFunKTestRunner::Init(UFunKEngineSubsystem* funKEngineSubsystem, EFunKTestRunnerType RunType)
{
	Type = RunType;
	FunKEngineSubsystem = funKEngineSubsystem;

	GetSinks(Sinks);
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

		bool isWrongEnvironment = false;
		const bool isEnvironmentRunning = IsEnvironmentRunning(Instructions, isWrongEnvironment);
		const bool isDifferentEnvironment = IsDifferentEnvironment(Instructions);
		if(isDifferentEnvironment || !isEnvironmentRunning || isWrongEnvironment)
		{
			if(isWrongEnvironment || (isDifferentEnvironment && isEnvironmentRunning))
			{
				GEditor->EndPlayMap();
				GEditor->EndPlayOnLocalPc();
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
			if(GetCurrentWorldController())
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
		UpdateState(EFunKTestRunnerState::Started);
		return true;
	}

	return false;
}

void UFunKTestRunner::End()
{
	UpdateState(EFunKTestRunnerState::Ended);
	CurrentTestWorld = nullptr;
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
	for (TScriptInterface<IFunKSink> FunKSink : Sinks)
	{
		if(FunKSink)
		{
			FunKSink->RaiseEvent(raisedEvent);
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

void UFunKTestRunner::RaiseStartEvent()
{
	RaiseInfoEvent("FunK Start");
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
	if(IsRunningTestUnderOneProcess)
	{
		const TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();
		for (auto& Context : WorldContexts)
		{
			if (Context.World())
			{
				FString WorldPackage = Context.World()->GetOutermost()->GetName();

				if (Context.WorldType == EWorldType::PIE)
				{
					isWrongEnvironmentRunning = Instructions.MapPackageName != UWorld::StripPIEPrefixFromPackageName(WorldPackage, Context.World()->StreamingLevelsPrefix);
					return true;
				}
			}
		}

		return false;
	}
	else
	{
		check(false);//TODO: Fix this!
		const bool isEnvironmentRunning = CurrentTestWorld.IsValid() && !CurrentTestWorld->bIsTearingDown;
		if(isEnvironmentRunning)
			isWrongEnvironmentRunning = CurrentTestWorld->GetMapName() != Instructions.MapPackageName;

		return isEnvironmentRunning;
	}
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
	
	// GEditor->EndPlayMap();

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

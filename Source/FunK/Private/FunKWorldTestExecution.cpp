// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestExecution.h"
#include "FunKFunctionalTest.h"
#include "FunKSettingsObject.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Sinks/FunKSink.h"


FString UFunKWorldTestExecution::FunKTestLifeTimeBeginEvent = FString("FunKTestLifeTimeBeginEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeBeginStageEvent = FString("FunKTestLifeTimeBeginStageEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeFinishStageEvent = FString("FunKTestLifeTimeFinishStageEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeFinishEvent = FString("FunKTestLifeTimeFinishEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeAllTestExecutionsFinishedEvent = FString("FunKTestLifeTimeAllTestExecutionsFinishedEvent");

void UFunKWorldTestExecution::Start(const UWorld* world, const TArray<AFunKTestBase*>& testsToExecute, TScriptInterface<IFunKSink> reportSink, FGuid executionId)
{
	const ENetMode netMode = world->GetNetMode();
	check(netMode != ENetMode::NM_Client)
	
	MasterController = world->GetSubsystem<UFunKWorldSubsystem>()->GetLocalTestController();
	check(!MasterController->CurrentTestExecution)
	
	TestsToExecute = testsToExecute;
	ReportSink = reportSink;
	ThisExecutionID = executionId.ToString();

	MasterController->CurrentTestExecution = this;
	for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
	{
		SpawnedController->CurrentTestExecution = this;
	}
	
	Settings = GetDefault<UFunKSettingsObject>();

	TotalTime = 0.f;
	IsAnyStarted = true;
	IsAllFinished = false;
	NextTest();
}

bool UFunKWorldTestExecution::IsTimeout(const FFunKTimeLimit* limit, float time)
{
	return limit && limit->IsTimeout(time);
}

bool UFunKWorldTestExecution::IsFinished() const
{
	return IsAllFinished;
}

AFunKWorldTestController* UFunKWorldTestExecution::GetMasterController() const
{
	return MasterController;
}

void UFunKWorldTestExecution::NextTest()
{
	PendingNextTest = false;
	PendingNextStage = false;
	
	CurrentExecutions.Empty(CurrentExecutions.Num());
	
	if(IsAllFinished)
		return;

	CurrentTestIndex++;
	if(CurrentTestIndex >= TestsToExecute.Num())
	{
		Finish();
		return;
	}

	CurrentStageIndex = -1;
	PendingStageTime = -1.f;
	PendingSyncTime = -1.f;

	RunTest(GetCurrentTest());

	if(CurrentExecutions.Num() <= 0)
		NextTest();
	else
		NextStage();
}

void UFunKWorldTestExecution::NextTestAsync()
{
	PendingNextTest = true;
}

void UFunKWorldTestExecution::Finish()
{
	IsAllFinished = true;
	IsAnyStarted = false;
	
	RaiseEvent(FFunKEvent::Info("Test execution finished", FunKTestLifeTimeAllTestExecutionsFinishedEvent).AddToContext(ThisExecutionID));
	CurrentExecutions.Empty();
	ThisExecutionID.Empty();
	Settings = nullptr;

	if(MasterController->CurrentTestExecution == this)
		MasterController->CurrentTestExecution = nullptr;
	
	for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
	{
		if(SpawnedController->CurrentTestExecution == this)
			SpawnedController->CurrentTestExecution = nullptr;
	}
}

void UFunKWorldTestExecution::StartSync()
{
	if(PendingSyncTime < 0.f)
		PendingSyncTime = 0.f;
}

void UFunKWorldTestExecution::NextStage()
{
	PendingNextStage = false;
	
	CurrentStageIndex++;
	PendingStageTime = -1.f;
	PendingSyncTime = -1.f;

	AFunKTestBase* test = GetCurrentTest();
	if(!test)
	{
		OnTestExecutionCanceled("Current test not found");
		return;
	}
	
	const FFunKStages* stages = test->GetStages();
	if(!stages || stages->Stages.Num() == 0)
	{
		OnTestExecutionCanceled("Stages not found");
		return;
	}
	
	if(stages->Stages.Num() <= CurrentStageIndex)
	{
		bool isAnyExecutionForLastStageActive = false;
		for(int32 i = 0; i < CurrentExecutions.Num(); i++)
		{
			isAnyExecutionForLastStageActive = CurrentExecutions[i].LastStartedStage == CurrentStageIndex - 1;
			if(isAnyExecutionForLastStageActive) break;
		}
		
		if(isAnyExecutionForLastStageActive)
			StartSync();
		else
			NextTestAsync();
		
		return;
	}

	bool isAnyStageStarted = false;
	bool isExecutionFinished = true;
	const ENetMode netMode = MasterController->GetNetMode();
	const FFunKStage& stage = stages->Stages[CurrentStageIndex];

	// Using 2 iterations: So that if the server succeeds instantly we already know for what it has to wait...
	//	1. Mark all that will be executed
	//	2. Actually execute the stages
	for (FFunKTestExecutionState& state : CurrentExecutions)
	{
		state.LastFinishedStage = ((!stage.IsOnStandalone && state.NetMode == NM_Standalone) ||
			(!stage.IsOnDedicatedServer && state.NetMode == NM_DedicatedServer) ||
				(!stage.IsOnListenServer && state.NetMode == NM_ListenServer) ||
					(!stage.IsOnListenServerClient && netMode == NM_ListenServer && state.NetMode == NM_Client) ||
						(!stage.IsOnDedicatedServerClient && netMode == NM_DedicatedServer && state.NetMode == NM_Client)) ? CurrentStageIndex : state.LastFinishedStage;
	}
	
	for(int32 i = 0; i < CurrentExecutions.Num(); i++)
	{
		FFunKTestExecutionState& state = CurrentExecutions[i];

		isExecutionFinished = isExecutionFinished && state.IsExecutionFinished;
		if(state.LastFinishedStage < CurrentStageIndex && !state.IsExecutionFinished)
		{
			isAnyStageStarted = true;

			state.LastStartedStage = CurrentStageIndex;
			state.Controller->BeginLocalTestStage(test, CurrentStageIndex);
			
			if(stage.IsLatent)
				PendingStageTime = 0.f;
			else 
				StartSync();
		}
	}

	if(!isAnyStageStarted && !isExecutionFinished)
	{
		NextStage();
	}
}

void UFunKWorldTestExecution::NextStageAsync()
{
	PendingNextStage = true;
}

void UFunKWorldTestExecution::RunTest(AFunKTestBase* test)
{
	const ENetMode netMode = MasterController->GetNetMode();
	int32 seed = FMath::Rand();
	
	if(netMode == NM_Standalone)
	{
		if(test->IsStandaloneModeTest())
			RunTestOnController(test, MasterController, netMode, seed);
	}
	else
	{
		const bool executeServer = (netMode == NM_DedicatedServer && test->IsRunOnDedicatedServer()) || (netMode == NM_ListenServer && test->IsRunOnListenServer());
		const bool executeClients = (netMode == NM_DedicatedServer && test->IsRunOnDedicatedServerClients()) || (netMode == NM_ListenServer && test->IsRunOnListenServerClients());
		
		if(executeServer)
			RunTestOnController(test, MasterController, netMode, seed);

		if(executeClients) for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
		{
			RunTestOnController(test, SpawnedController, ENetMode::NM_Client, seed);
		}
	}
}

void UFunKWorldTestExecution::RunTestOnController(AFunKTestBase* test, AFunKWorldTestController* controller, const ENetMode netMode, int32 Seed)
{
	const FGuid guid = FGuid::NewGuid();
	FFunKTestExecutionState& state = CurrentExecutions.Add_GetRef(FFunKTestExecutionState());
	state.Controller = controller;
	state.Id = guid;
	state.NetMode = netMode;
	
	controller->BeginLocalTest(test, guid, Seed);
}

AFunKTestBase* UFunKWorldTestExecution::GetCurrentTest()
{
	if(CurrentTestIndex >= TestsToExecute.Num())
		return nullptr;
	
	return TestsToExecute[CurrentTestIndex];
}

void UFunKWorldTestExecution::OnTestStageFinished(const FFunKEvent& raisedEvent)
{
	if(FFunKTestExecutionState* state = GetStateFromEvent(raisedEvent))
	{
		OnTestStageFinished(state);
	}
}

void UFunKWorldTestExecution::OnTestStageFinished(FFunKTestExecutionState* State)
{
	State->LastFinishedStage = CurrentStageIndex;
	
	for(int32 i = 0; i < CurrentExecutions.Num(); i++)
	{
		if(CurrentExecutions[i].LastFinishedStage < CurrentStageIndex)
			return;
	}

	OnTestStageFinished();
}

void UFunKWorldTestExecution::OnTestStageFinished()
{
	NextStageAsync();
}

void UFunKWorldTestExecution::OnTestFinished(const FFunKEvent& raisedEvent)
{
	if(FFunKTestExecutionState* state = GetStateFromEvent(raisedEvent))
	{
		OnTestFinished(state);
	}
}

void UFunKWorldTestExecution::OnTestFinished(FFunKTestExecutionState* State)
{
	State->IsExecutionFinished = true;

	for(int32 i = 0; i < CurrentExecutions.Num(); i++)
	{
		if(!CurrentExecutions[i].IsExecutionFinished)
			return;
	}

	OnTestFinished();
}

void UFunKWorldTestExecution::OnTestFinished()
{
	RaiseEvent(FFunKEvent::Info("Finished Test acknowledged", GetCurrentTest()->GetName()).AddToContext(ThisExecutionID));
	NextTestAsync();
}

void UFunKWorldTestExecution::OnTestExecutionCanceled(const FString& ReasonMessage)
{
	const AFunKTestBase* currentTest = GetCurrentTest();
	
	RaiseEvent(FFunKEvent::Error("Canceled Test: " + ReasonMessage, currentTest ? GetCurrentTest()->GetName() : "Missing Test").AddToContext(ThisExecutionID));
	NextTest();
}

FFunKTestExecutionState* UFunKWorldTestExecution::GetStateFromEvent(const FFunKEvent& raisedEvent)
{
	for(int32 i = 0; i < CurrentExecutions.Num(); i++)
	{
		if(raisedEvent.Context.Contains(CurrentExecutions[i].Id.ToString()))
			return &CurrentExecutions[i];
	}
	
	return nullptr;
}

void UFunKWorldTestExecution::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	ReportSink->RaiseEvent(raisedEvent);
	
	if(raisedEvent.Context.Contains(FunKTestLifeTimeFinishStageEvent))
	{
		UFunKWorldTestExecution* mutableThis = const_cast<UFunKWorldTestExecution*>(this);
		mutableThis->OnTestStageFinished(raisedEvent);
	}
	else if(raisedEvent.Context.Contains(FunKTestLifeTimeFinishEvent))
	{
		UFunKWorldTestExecution* mutableThis = const_cast<UFunKWorldTestExecution*>(this);
		mutableThis->OnTestFinished(raisedEvent);
	}
}

void UFunKWorldTestExecution::Tick(float DeltaTime)
{
	// Apparently the tick function can be invoked more than once per tick
	if ( LastTickFrame == GFrameCounter )
		return;

	LastTickFrame = GFrameCounter;
	
	if(IsFinished())
		return;
	
	TotalTime += DeltaTime;

	// This is kinda jank... But in none latent stages the instant result in standalone (and under certain circumstances in server) scenarios leads to unexpected (& unwanted) behaviour.
	if(PendingNextTest)
	{
		NextTest();
		return;
	}
	
	if(PendingNextStage)
	{
		NextStage();
		return;
	}

	if(PendingSyncTime >= 0.f)
	{
		PendingSyncTime += DeltaTime;
		if(const AFunKTestBase* currentTest = GetCurrentTest())
		{
			const FFunKTimeLimit* limit = &Settings->Settings.SyncTimeLimit;
			if(IsTimeout(limit, PendingSyncTime))
			{
				RaiseEvent(currentTest->CreateEvent(limit->Result, limit->Message.ToString()));
				NextTest();
				return;
			}
		}
		else
		{
			OnTestExecutionCanceled("Could not be synced because the tests instance was deleted!");
			return;
		}
	}
	else
	{
		PendingStageTime += DeltaTime;
		if(AFunKTestBase* currentTest = GetCurrentTest())
		{
			const FFunKTimeLimit& currentStageTimeLimit = currentTest->GetStages()->Stages[CurrentStageIndex].TimeLimit;
			if(IsTimeout(&currentStageTimeLimit, PendingStageTime))
			{
				for(int32 i = 0; i < CurrentExecutions.Num(); i++)
				{
					if(!CurrentExecutions[i].IsExecutionFinished)
					{
						CurrentExecutions[i].Controller->FinishLocalTest(currentTest, currentStageTimeLimit.Result, currentStageTimeLimit.Message.ToString());
					}
				}
				
				StartSync();
				return;
			}
		}
		else
		{
			OnTestExecutionCanceled("Could not be waited for because the tests instance was deleted!");
			return;
		}
	}
}

TStatId UFunKWorldTestExecution::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFunKWorldTestExecution, STATGROUP_Tickables);
}

bool UFunKWorldTestExecution::IsTickableInEditor() const
{
	return true;
}

UWorld* UFunKWorldTestExecution::GetTickableGameObjectWorld() const
{
	return MasterController->GetWorld();
}

ETickableTickType UFunKWorldTestExecution::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UFunKWorldTestExecution::IsTickable() const
{
	return !IsFinished() && IsAnyStarted;
}

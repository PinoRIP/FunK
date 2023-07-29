// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestExecution.h"

#include "FunKFunctionalTest.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Sinks/FunKSink.h"

FString UFunKWorldTestExecution::FunKTestLifeTimeStartEvent = FString("FunKTestLifeTimeStartEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimePreparationCompleteEvent = FString("FunKTestLifeTimePreparationCompleteEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeTestFinishedEvent = FString("FunKTestLifeTimeTestFinishedEvent");
FString UFunKWorldTestExecution::FunKTestLifeTimeTestExecutionFinishedEvent = FString("FunKTestLifeTimeTestExecutionFinishedEvent");

void UFunKWorldTestExecution::Start(UWorld* world, const TArray<AFunKTestBase*>& testsToExecute, TScriptInterface<IFunKSink> reportSink, FGuid executionId)
{
	const ENetMode netMode = world->GetNetMode();
	check(netMode != ENetMode::NM_Client)
	
	MasterController = world->GetSubsystem<UFunKWorldSubsystem>()->GetLocalTestController();
	check(!MasterController->CurrentTestExecution)
	
	TestsToExecute = testsToExecute;
	ReportSink = reportSink;
	ThisExecutionId = executionId.ToString();

	MasterController->CurrentTestExecution = this;
	for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
	{
		SpawnedController->CurrentTestExecution = this;
	}
	
	//TODO: Maybe we want this class to tick on its own?
	MasterController->SetActorTickEnabled(true);

	TotalTime = 0.f;
	IsAllFinished = false;
	NextTest();
}

void UFunKWorldTestExecution::Update(float DeltaTime)
{
	if(IsAllFinished)
		return;
	
	TotalTime += DeltaTime;

	if(PendingNetworkingTime >= 0.f)
	{
		PendingNetworkingTime += DeltaTime;
		AFunKTestBase* currentTest = GetCurrentTest();
		if(!currentTest || IsTimeout(currentTest->GetNetworkingTimeLimit(), PendingNetworkingTime))
		{
			const FFunKTimeLimit* limit = currentTest->GetNetworkingTimeLimit();
			RaiseEvent(currentTest->CreateEvent(limit->Result, limit->Message.ToString()));
			NextTest();
			return;
		}
	}
	else if(ExecutionTime < 0.f)
	{
		PreparationTime += DeltaTime;
		if(AFunKTestBase* currentTest = GetCurrentTest())
		{
			if(HandleTimeout(currentTest, currentTest->GetPreparationTimeLimit(), PreparationTime))
			{
				PendingNetworkingTime = 0.f;
				return;
			}
		}
		else
		{
			RaiseEvent(FFunKEvent::Error("Canceled test because the tests instance was deleted!", ThisExecutionId));
			NextTest();
			return;
		}
	}
	else
	{
		ExecutionTime += DeltaTime;
		if(AFunKTestBase* currentTest = GetCurrentTest())
		{
			if(HandleTimeout(currentTest, currentTest->GetTimeLimit(), ExecutionTime))
			{
				PendingNetworkingTime = 0.f;
				return;
			}
		}
		else
		{
			RaiseEvent(FFunKEvent::Error("Canceled test because the tests instance was deleted!", ThisExecutionId));
			NextTest();
			return;
		}
	}
}

bool UFunKWorldTestExecution::HandleTimeout(AFunKTestBase* currentTest, const FFunKTimeLimit* limit, float time)
{
	if(IsTimeout(limit, time))
	{
		const bool isPreparation = currentTest->GetPreparationTimeLimit() == limit;
		for(int32 i = 0; i < CurrentExecutions.Num(); i++)
		{
			if(!CurrentExecutions[i].IsFinished())
			{
				CurrentExecutions[i].Controller->FinishLocalTest(currentTest, isPreparation && CurrentExecutions[i].IsSetup() ? EFunKFunctionalTestResult::Skipped : limit->Result, limit->Message.ToString());
			}
		}

		return true;
	}

	return false;
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

bool UFunKWorldTestExecution::IsExecutionFinishedEvent(const FFunKEvent& raisedEvent, FFunKTestID ExecutionId)
{
	return true;// raisedEvent.Context.Contains(FunKTestLifeTimeTestExecutionFinishedEvent) && raisedEvent.Context.Contains(ExecutionId);
}

void UFunKWorldTestExecution::NextTest()
{
	CurrentExecutions.Empty(CurrentExecutions.Num());
	
	if(IsAllFinished)
		return;

	CurrentTest++;
	if(CurrentTest >= TestsToExecute.Num())
	{
		Finish();
		return;
	}
	
	PreparationTime = 0.f;
	ExecutionTime = -1.f;
	PendingNetworkingTime = -1.f;

	RunTest(GetCurrentTest());

	if(CurrentExecutions.Num() <= 0)
		NextTest();
}

void UFunKWorldTestExecution::Finish()
{
	IsAllFinished = true;
	RaiseEvent(FFunKEvent::Info("Test execution finished", FunKTestLifeTimeTestExecutionFinishedEvent).AddToContext(ThisExecutionId));
	MasterController->SetActorTickEnabled(false);
	CurrentExecutions.Empty();

	if(MasterController->CurrentTestExecution == this)
		MasterController->CurrentTestExecution = nullptr;
	for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
	{
		if(SpawnedController->CurrentTestExecution == this)
			SpawnedController->CurrentTestExecution = nullptr;
	}
}

void UFunKWorldTestExecution::RunTest(AFunKTestBase* test)
{
	const ENetMode netMode = MasterController->GetNetMode();
	if(netMode == NM_Standalone)
	{
		if(test->IsStandaloneModeTest())
			RunTestOnController(test, MasterController);
	}
	else
	{
		const bool executeServer = (netMode == NM_DedicatedServer && test->IsRunOnDedicatedServer()) || (netMode == NM_ListenServer && test->IsRunOnListenServer());
		const bool executeClients = (netMode == NM_DedicatedServer && test->IsRunOnDedicatedServerClients()) || (netMode == NM_ListenServer && test->IsRunOnListenServerClients());
		
		if(executeServer)
			RunTestOnController(test, MasterController);

		if(executeClients) for (AFunKWorldTestController* SpawnedController : MasterController->SpawnedController)
		{
			RunTestOnController(test, SpawnedController);
		}
	}
}

FFunKWorldTestState& UFunKWorldTestExecution::RunTestOnController(AFunKTestBase* test, AFunKWorldTestController* controller)
{
	const FGuid guid = FGuid::NewGuid();
	FFunKWorldTestState& state = CurrentExecutions.Add_GetRef(FFunKWorldTestState());
	state.Controller = controller;
	state.Id = guid;
	
	controller->BeginLocalTestSetup(test, guid);
	
	return state;
}

AFunKTestBase* UFunKWorldTestExecution::GetCurrentTest()
{
	if(CurrentTest >= TestsToExecute.Num())
		return nullptr;
	
	return TestsToExecute[CurrentTest];
}

void UFunKWorldTestExecution::OnTestPreparationComplete(const FFunKEvent& raisedEvent)
{
	if(FFunKWorldTestState* state = GetStateFromEvent(raisedEvent))
	{
		state->SetupCompleteTime = PreparationTime;
		for(int32 i = 0; i < CurrentExecutions.Num(); i++)
		{
			if(!CurrentExecutions[i].IsSetup())
				return;
		}

		OnAllTestsPreparationsComplete();
	}
}

void UFunKWorldTestExecution::OnAllTestsPreparationsComplete()
{
	ExecutionTime = 0.f;
	for(int32 i = 0; i < CurrentExecutions.Num(); i++)
	{
		if(!CurrentExecutions[i].IsFinished())
			CurrentExecutions[i].Controller->BeginLocalTestExecution(GetCurrentTest());
	}
}

void UFunKWorldTestExecution::OnTestExecutionComplete(const FFunKEvent& raisedEvent)
{
	if(FFunKWorldTestState* state = GetStateFromEvent(raisedEvent))
	{
		if(!state->IsSetup())
		{
			state->SetupCompleteTime = PreparationTime;
			state->FinishedTime = FMath::Max(0, ExecutionTime);
		}
		else
		{
			state->FinishedTime = ExecutionTime;
		}

		for(int32 i = 0; i < CurrentExecutions.Num(); i++)
		{
			if(!CurrentExecutions[i].IsFinished())
				return;
		}

		OnAllTestsExecutionsComplete();
	}
}

void UFunKWorldTestExecution::OnAllTestsExecutionsComplete()
{
	RaiseEvent(FFunKEvent::Info("Finished Test", GetCurrentTest()->GetName()).AddToContext(ThisExecutionId));
	NextTest();
}

FFunKWorldTestState* UFunKWorldTestExecution::GetStateFromEvent(const FFunKEvent& raisedEvent)
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
	
	if(raisedEvent.Context.Contains(FunKTestLifeTimePreparationCompleteEvent))
	{
		UFunKWorldTestExecution* mutableThis = const_cast<UFunKWorldTestExecution*>(this);
		mutableThis->OnTestPreparationComplete(raisedEvent);
	}
	else if(raisedEvent.Context.Contains(FunKTestLifeTimeTestFinishedEvent))
	{
		UFunKWorldTestExecution* mutableThis = const_cast<UFunKWorldTestExecution*>(this);
		mutableThis->OnTestExecutionComplete(raisedEvent);
	}
}

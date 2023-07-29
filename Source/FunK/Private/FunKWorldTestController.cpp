// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"

#include "EngineUtils.h"
#include "FunKFunctionalTest.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "FunKWorldSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFunKWorldTestController::AFunKWorldTestController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("TEST");
}

void AFunKWorldTestController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( AFunKWorldTestController, ActiveController );
	DOREPLIFETIME( AFunKWorldTestController, ControllerIndex );
	DOREPLIFETIME( AFunKWorldTestController, IsServerDedicated );
}

// Called when the game starts or when spawned
void AFunKWorldTestController::BeginPlay()
{
	Super::BeginPlay();

	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Client)
	{
		CheckControllerReady();
	}
	else
	{
		ControllerReady();
		IsServerDedicated = GetNetMode() == NM_DedicatedServer;
	}
	
	SetActorTickEnabled(false);
}

void AFunKWorldTestController::CreateTestControllerForClient(APlayerController* NewPlayer)
{
	if(GetOwner() == NewPlayer)
		return;

	for (const AFunKWorldTestController* Controller : SpawnedController)
	{
		if(Controller->GetOwner() == NewPlayer)
			return;
	}
	
	AFunKWorldTestController* NewController = GetWorld()->SpawnActor<AFunKWorldTestController>(GetClass());
	NewController->SetOwner(NewPlayer);
	SpawnedController.Add(NewController);
	NewController->ControllerIndex = SpawnedController.Num();
}

void AFunKWorldTestController::BeginDestroy()
{
	Super::BeginDestroy();
	FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
}

void AFunKWorldTestController::OnRep_Owner()
{
	Super::OnRep_Owner();

	if(GetNetMode() == NM_Client && GetOwner() == GetWorld()->GetFirstPlayerController())
	{
		if(UFunKWorldSubsystem* funk = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
		{
			funk->SetLocalTestController(this);
			CheckControllerReady();
		}
	}
}

void AFunKWorldTestController::ControllerReady()
{
	ActiveController++;
	for (AFunKWorldTestController* Controller : SpawnedController)
	{
		Controller->ActiveController = ActiveController;
	}
}

void AFunKWorldTestController::SetupLocalTestController()
{
	const ENetMode netMode = GetNetMode();
	if(netMode == NM_ListenServer || netMode == NM_DedicatedServer)
	{
		FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &AFunKWorldTestController::OnConnection);
	}
}

bool AFunKWorldTestController::IsControllerLocallyReady() const
{
	return HasActorBegunPlay() && GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->HasLocalTestController() && (GetNetMode() != NM_Client || GetOwner() == GetWorld()->GetFirstPlayerController());
}

void AFunKWorldTestController::CheckControllerReady()
{
	if(IsControllerLocallyReady() && !IsControllerReadinessSend)
	{
		IsControllerReadinessSend = true;
		ServerControllerReady();
	}
}

void AFunKWorldTestController::OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	CreateTestControllerForClient(NewPlayer);
}

void AFunKWorldTestController::ServerExecuteTestByName_Implementation(const FString& TestName, FGuid ExecutionId)
{
	ExecuteTestByName(TestName, this, ExecutionId);
}

void AFunKWorldTestController::ServerExecuteAllTests_Implementation(FGuid ExecutionId)
{
	ExecuteAllTests(this, ExecutionId);
}

void AFunKWorldTestController::ExecuteTests(const TArray<AFunKTestBase*>& TestToExecute, TScriptInterface<IFunKSink> ReportSink, FGuid executionId)
{
	if(!CurrentTestExecution)
	{
		NewObject<UFunKWorldTestExecution>()->Start(GetWorld(), TestToExecute, ReportSink, executionId);
	}
}

void AFunKWorldTestController::ExecuteTestByName(const FString& TestName, TScriptInterface<IFunKSink> ReportSink, FGuid ExecutionId)
{
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		if(TestName == FunctionalTest->GetName())
		{
			TArray<AFunKTestBase*> Test;
			Test.Add(FunctionalTest);
			ExecuteTests(Test, ReportSink, ExecutionId);
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink, FGuid ExecutionId)
{
	TArray<AFunKTestBase*> Test;
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		Test.Add(FunctionalTest);
	}

	ExecuteTests(Test, ReportSink, ExecutionId);
}

void AFunKWorldTestController::ClientBeginLocalTestSetup_Implementation(AFunKTestBase* TestToExecute, FFunKTestID ExecutionId)
{
	if(TestToExecute)
	{
		BeginLocalTestSetup(TestToExecute, ExecutionId);
	}
	else
	{
		RaiseEvent(FFunKEvent::Error("Could not invoke test actor!", "AFunKWorldTestController::ClientExecuteTest_Implementation").AddToContext(ExecutionId.ToString()));
	}
}

void AFunKWorldTestController::BeginLocalTestSetup(AFunKTestBase* TestToExecute, FGuid ExecutionId)
{
	if(IsLocalTestController())
	{
		TestToExecute->BeginTest(this, ExecutionId);
	}
	else if(GetNetMode() != NM_Client)
	{
		ClientBeginLocalTestSetup(TestToExecute, ExecutionId);
	}
}

void AFunKWorldTestController::ClientBeginLocalTestExecution_Implementation(AFunKTestBase* TestToExecute)
{
	if(TestToExecute)
	{
		BeginLocalTestExecution(TestToExecute);
	}
	else
	{
		RaiseEvent(FFunKEvent::Error("TestToExecute could not be found!"));
	}
}

void AFunKWorldTestController::BeginLocalTestExecution(AFunKTestBase* TestToExecute)
{
	if(IsLocalTestController())
	{
		TestToExecute->BeginTestStage();
	}
	else if(GetNetMode() != NM_Client)
	{
		ClientBeginLocalTestExecution(TestToExecute);
	}
}

void AFunKWorldTestController::ClientFinishLocalTest_Implementation(AFunKTestBase* TestToCancel, EFunKFunctionalTestResult Result, const FString& Message)
{
	if(TestToCancel)
	{
		FinishLocalTest(TestToCancel, Result, Message);
	}
	else
	{
		RaiseEvent(FFunKEvent::Error("TestToCancel could not be found!"));
	}
}

void AFunKWorldTestController::FinishLocalTest(AFunKTestBase* TestToCancel, EFunKFunctionalTestResult Result, const FString& Message)
{
	if(IsLocalTestController())
	{
		TestToCancel->FinishTest(Result, Message);
	}
	else if(GetNetMode() != NM_Client)
	{
		ClientFinishLocalTest(TestToCancel, Result, Message);
	}
}

void AFunKWorldTestController::ServerSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const
{
	CurrentTestExecution->RaiseEvent(FFunKEvent(eventType, Message, Context));
}

void AFunKWorldTestController::ClientSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const
{
	if(LocalReportSink)
	{
		LocalReportSink->RaiseEvent(FFunKEvent(eventType, Message, Context));
		if(Context.Contains(UFunKWorldTestExecution::FunKTestLifeTimeTestExecutionFinishedEvent) && Context.Contains(ExpectedProxyEvent))
		{
			//TODO: This not cool... FixThis
			AFunKWorldTestController* mutableThis = const_cast<AFunKWorldTestController*>(this);
			mutableThis->ExpectedProxyEvent = "";
		}
	}
}

void AFunKWorldTestController::ServerControllerReady_Implementation() const
{
	if(UFunKWorldSubsystem* funkWorldSubsystem = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
	{
		funkWorldSubsystem->GetLocalTestController()->ControllerReady();
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("FunKWorldSubsystem not found..."));
	}
}

// Called every frame
void AFunKWorldTestController::Tick(float DeltaTime)
{
	if(CurrentTestExecution && CurrentTestExecution->GetMasterController() == this)
	{
		CurrentTestExecution->Update(DeltaTime);
	}
	
	Super::Tick(DeltaTime);
}

void AFunKWorldTestController::ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> reportSink)
{
	const ENetMode netMode = GetNetMode();
	const FGuid executionId = FGuid::NewGuid();
	
	if(netMode == NM_Client)
	{
		LocalReportSink = reportSink;
		ExpectedProxyEvent = executionId.ToString();
		ServerExecuteTestByName(TestName, executionId);
	}
	else
		ExecuteTestByName(TestName, reportSink, executionId);
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> reportSink)
{
	const ENetMode netMode = GetNetMode();
	const FGuid executionId = FGuid::NewGuid();
	
	if(netMode == NM_Client)
	{
		LocalReportSink = reportSink;
		ExpectedProxyEvent = executionId.ToString();
		ServerExecuteAllTests(executionId);
	}
	else
		ExecuteAllTests(reportSink, executionId);
}

bool AFunKWorldTestController::IsFinished() const
{
	return CurrentTestExecution
		? CurrentTestExecution->IsFinished()
		: ExpectedProxyEvent.IsEmpty();
}

void AFunKWorldTestController::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	const ENetMode netMode = GetNetMode();
	if(IsLocalTestController())
	{
		if(netMode == NM_Client)
			ServerSendEvent(raisedEvent.Type, raisedEvent.Message, raisedEvent.Context);
		else
		{
			if(CurrentTestExecution)
			{
				CurrentTestExecution->RaiseEvent(raisedEvent);
			}
			else
			{
				UE_LOG(FunKLog, Error, TEXT("Failed to RaiseEvent due to a missing test execution: %s"), *GetRoleName());
			}
		}
	}
	else if(netMode == NM_DedicatedServer || netMode == NM_ListenServer)
	{
		ClientSendEvent(raisedEvent.Type, raisedEvent.Message, raisedEvent.Context);
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("World test controller can only act as sinks when they are the local test controller"));
	}
}

FString AFunKWorldTestController::GetRoleName() const
{
	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Standalone)
		return "Standalone";

	if(ControllerIndex == INDEX_NONE)
		return "Server";
	
	return "Client_" + FString::FromInt(ControllerIndex);
}

int32 AFunKWorldTestController::GetActiveControllerCount() const
{
	return ActiveController;
}

int32 AFunKWorldTestController::GetControllerIndex() const
{
	return ControllerIndex;
}

bool AFunKWorldTestController::IsLocalTestController() const
{
	return GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->GetLocalTestController() == this;
}


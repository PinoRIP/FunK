// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"

#include "EngineUtils.h"
#include "FunKFunctionalTest.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestExecution.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFunKWorldTestController::AFunKWorldTestController()
{
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
		OnControllerReady();
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

void AFunKWorldTestController::OnControllerReady()
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

void AFunKWorldTestController::ServerExecuteTestByName_Implementation(const FString& TestName, const FFunKTestRunID TestRunID)
{
	ExecuteTestByName(TestName, this, TestRunID);
}

void AFunKWorldTestController::ServerExecuteAllTests_Implementation(FFunKTestRunID TestRunID)
{
	ExecuteAllTests(this, TestRunID);
}

void AFunKWorldTestController::ExecuteTests(const TArray<AFunKTestBase*>& TestToExecute, TScriptInterface<IFunKSink> ReportSink, const FFunKTestRunID TestRunID)
{
	if(!CurrentTestExecution)
	{
		NewObject<UFunKWorldTestExecution>()->Start(GetWorld(), TestToExecute, ReportSink, TestRunID);
	}
}

void AFunKWorldTestController::ExecuteTestByName(const FString& TestName, TScriptInterface<IFunKSink> ReportSink, const FFunKTestRunID TestRunID)
{
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		if(TestName == FunctionalTest->GetName())
		{
			TArray<AFunKTestBase*> Test;
			Test.Add(FunctionalTest);
			ExecuteTests(Test, ReportSink, TestRunID);
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink, const FFunKTestRunID TestRunID)
{
	TArray<AFunKTestBase*> Test;
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		Test.Add(FunctionalTest);
	}

	ExecuteTests(Test, ReportSink, TestRunID);
}

void AFunKWorldTestController::ClientBeginLocalTest_Implementation(AFunKTestBase* TestToBegin, FFunKTestRunID TestRunID)
{
	if(TestToBegin)
	{
		BeginLocalTest(TestToBegin, TestRunID);
	}
	else
	{
		RaiseEvent(FFunKEvent::Error("Could not invoke test actor!", "AFunKWorldTestController::ClientExecuteTest_Implementation").AddToContext(TestRunID.ToString()));
	}
}

void AFunKWorldTestController::BeginLocalTest(AFunKTestBase* TestToBegin, FFunKTestRunID TestRunID)
{
	if(IsLocalTestController())
	{
		TestToBegin->BeginTest(this, TestRunID);
	}
	else if(GetNetMode() != NM_Client)
	{
		ClientBeginLocalTest(TestToBegin, TestRunID);
	}
}

void AFunKWorldTestController::ClientBeginLocalTestStage_Implementation(AFunKTestBase* TestToExecute, int32 StageIndex)
{
	if(TestToExecute)
	{
		BeginLocalTestStage(TestToExecute, StageIndex);
	}
	else
	{
		RaiseEvent(FFunKEvent::Error("TestToExecute could not be found!"));
	}
}

void AFunKWorldTestController::BeginLocalTestStage(AFunKTestBase* TestToExecute, int32 StageIndex)
{
	if(IsLocalTestController())
	{
		TestToExecute->BeginTestStage(StageIndex);
	}
	else if(GetNetMode() != NM_Client)
	{
		ClientBeginLocalTestStage(TestToExecute, StageIndex);
	}
}

void AFunKWorldTestController::ClientFinishLocalTest_Implementation(AFunKTestBase* TestToCancel, EFunKTestResult Result, const FString& Message)
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

void AFunKWorldTestController::FinishLocalTest(AFunKTestBase* TestToCancel, EFunKTestResult Result, const FString& Message)
{
	if(IsLocalTestController())
	{
		TestToCancel->FinishStage(Result, Message);
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
		if(Context.Contains(UFunKWorldTestExecution::FunKTestLifeTimeAllTestExecutionsFinishedEvent) && Context.Contains(RemoteTestRunID))
		{
			//TODO: This not cool... FixThis
			AFunKWorldTestController* mutableThis = const_cast<AFunKWorldTestController*>(this);
			mutableThis->RemoteTestRunID = "";
		}
	}
}

void AFunKWorldTestController::ServerControllerReady_Implementation() const
{
	if(UFunKWorldSubsystem* funkWorldSubsystem = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
	{
		funkWorldSubsystem->GetLocalTestController()->OnControllerReady();
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("FunKWorldSubsystem not found..."));
	}
}

// Called every frame
void AFunKWorldTestController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFunKWorldTestController::ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> InReportSink)
{
	const ENetMode netMode = GetNetMode();
	const FFunKTestRunID TestRunID = FFunKTestRunID::NewGuid();
	
	if(netMode == NM_Client)
	{
		LocalReportSink = InReportSink;
		RemoteTestRunID = TestRunID.ToString();
		ServerExecuteTestByName(TestName, TestRunID);
	}
	else
		ExecuteTestByName(TestName, InReportSink, TestRunID);
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> InReportSink)
{
	const ENetMode NetMode = GetNetMode();
	const FFunKTestRunID TestRunID = FFunKTestRunID::NewGuid();
	
	if(NetMode == NM_Client)
	{
		LocalReportSink = InReportSink;
		RemoteTestRunID = TestRunID.ToString();
		ServerExecuteAllTests(TestRunID);
	}
	else
		ExecuteAllTests(InReportSink, TestRunID);
}

bool AFunKWorldTestController::IsFinished() const
{
	return CurrentTestExecution
		? CurrentTestExecution->IsFinished()
		: RemoteTestRunID.IsEmpty();
}

void AFunKWorldTestController::RaiseEvent(const FFunKEvent& RaisedEvent) const
{
	const ENetMode netMode = GetNetMode();
	if(IsLocalTestController())
	{
		if(netMode == NM_Client)
			ServerSendEvent(RaisedEvent.Type, RaisedEvent.Message, RaisedEvent.Context);
		else
		{
			if(CurrentTestExecution)
			{
				CurrentTestExecution->RaiseEvent(RaisedEvent);
			}
			else
			{
				UE_LOG(FunKLog, Error, TEXT("Failed to RaiseEvent due to a missing test execution: %s"), *GetRoleName());
			}
		}
	}
	else if(netMode == NM_DedicatedServer || netMode == NM_ListenServer)
	{
		ClientSendEvent(RaisedEvent.Type, RaisedEvent.Message, RaisedEvent.Context);
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


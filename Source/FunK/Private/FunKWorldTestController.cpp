// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"

#include "EngineUtils.h"
#include "FunKEngineSubsystem.h"
#include "FunKFunctionalTest.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "FunKWorldSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Sinks/FunKLogSink.h"


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

void AFunKWorldTestController::ExecuteTest(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> reportSink, FGuid executionId)
{
	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Standalone)
	{
		ExecuteTestRun(TestToExecute, reportSink, executionId);
	}
	else if(netMode == NM_Client)
	{
		ReportSink = reportSink;
		
		SubExecutionIds.Add(executionId);
		ServerExecuteTest(TestToExecute, executionId);
	}
	else
	{
		ExecuteTestRun(TestToExecute, reportSink, executionId);

		for (AFunKWorldTestController* Controller : SpawnedController)
		{
			if(Controller != reportSink.GetObject())
				Controller->ReportSink = this;

			executionId = FGuid::NewGuid();
			SubExecutionIds.Add(executionId);
			Controller->ClientExecuteTest(TestToExecute, executionId);
		}
	}
}

void AFunKWorldTestController::FinishedCurrentLocalTest()
{
	RaiseInfoEvent("Finished!", CurrentExecutionId.ToString());
	NetworkingTotalTime = 0.f;
}

void AFunKWorldTestController::FinishedCurrentTest()
{
	SetActorTickEnabled(false);
	ReportSink = nullptr;
	CurrentTest = nullptr;

	// This should be empty already, but in cases where we cancel this needs to be reset.
	SubExecutionIds.Empty(SubExecutionIds.Num());
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

void AFunKWorldTestController::ServerExecuteTest_Implementation(AFunKFunctionalTest* TestToExecute, FGuid ExecutionId)
{
	if(TestToExecute)
	{
		if(UFunKWorldSubsystem* funkWorldSubsystem = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
		{
			funkWorldSubsystem->GetLocalTestController()->ExecuteTest(TestToExecute, this, ExecutionId);
		}
		else
		{
			UE_LOG(FunKLog, Error, TEXT("FunKWorldSubsystem not found..."));
		}
	}
	else
	{
		RaiseErrorEvent("Could not invoke test actor!", "AFunKWorldTestController::ServerExecuteTest_Implementation");
	}
}

void AFunKWorldTestController::ClientExecuteTest_Implementation(AFunKFunctionalTest* TestToExecute, FGuid ExecutionId)
{
	if(TestToExecute)
	{
		ExecuteTestRun(TestToExecute, nullptr, ExecutionId);
	}
	else
	{
		RaiseErrorEvent("Could not invoke test actor!", "AFunKWorldTestController::ClientExecuteTest_Implementation");
	}
}

void AFunKWorldTestController::ExecuteTestRun(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> reportSink, FGuid ExecutionId)
{
	if(!ReportSink)
		ReportSink = reportSink;
	
	CurrentTest = TestToExecute;
	CurrentExecutionId = ExecutionId;
	TotalTime = 0.f;
	NetworkingTotalTime = -1.f;
	
	SetActorTickEnabled(true);
	TestToExecute->RunTest(this);
}

void AFunKWorldTestController::SendEvent(const FFunKEvent& raisedEvent) const
{
	if(GetNetMode() == NM_Standalone)
	{
		UE_LOG(FunKLog, Error, TEXT("SendEvent event should never be reached when testing standalone!"))
	}
	
	if(GetNetMode() == NM_Client)
	{
		ServerSendEvent(raisedEvent.Type, raisedEvent.Message, raisedEvent.Context);
	}
	else
	{
		ClientSendEvent(raisedEvent.Type, raisedEvent.Message, raisedEvent.Context);
	}
}

void AFunKWorldTestController::ApplySendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const
{
	const FFunKEvent event = FFunKEvent(eventType, Message, Context);
	if(!ReportEvent(event))
	{
		UFunKLogSink::LogEvent(FFunKEvent(eventType, Message, Context));
	}
}

void AFunKWorldTestController::ServerSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const
{
	ApplySendEvent(eventType, Message, Context);
}

void AFunKWorldTestController::ClientSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const
{
	ApplySendEvent(eventType, Message, Context);
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

bool AFunKWorldTestController::ReportEvent(const FFunKEvent& raisedEvent) const
{
	if(ReportSink)
	{
		FulfillSubExecution(raisedEvent);
		ReportSink->RaiseEvent(raisedEvent);
		return true;
	}

	return false;
}

void AFunKWorldTestController::FulfillSubExecution(const FFunKEvent& raisedEvent) const
{
	if(SubExecutionIds.Num() > 0 && raisedEvent.Message.StartsWith("Finished"))
	{
		FGuid parsedExecutionId;
		for (const FString& Context : raisedEvent.Context)
		{
			if(FGuid::ParseExact(Context, EGuidFormats::Digits, parsedExecutionId))
			{
				//TODO: This not cool... FixThis
				AFunKWorldTestController* mutableThis = const_cast<AFunKWorldTestController*>(this);
				mutableThis->SubExecutionIds.Remove(parsedExecutionId);
			}
		}
		
		if(!parsedExecutionId.IsValid())
		{
			RaiseEvent(FFunKEvent::Error("Could not parse execution finished!", raisedEvent.Message).AddToContext(raisedEvent.Context));
		}
	}
}

// Called every frame
void AFunKWorldTestController::Tick(float DeltaTime)
{
	TotalTime += DeltaTime;

	if(CurrentTest)
	{
		if(IsLocallyFinished())
		{
			if(NetworkingTotalTime < 0)
			{
				FinishedCurrentLocalTest();
			}
			else
			{
				NetworkingTotalTime += DeltaTime;
				if(CurrentTest->NetworkingTimeLimit.IsTimeout(NetworkingTotalTime))
				{
					//TODO: This is currently a copy of the FunctionalTest-Class finalize function
					if(CurrentTest->NetworkingTimeLimit.Result == EFunKFunctionalTestResult::Succeeded)
					{
						RaiseInfoEvent(FString::Printf(TEXT("Netwoktimeout: %s"), *CurrentTest->NetworkingTimeLimit.Message.ToString()), GetName());
					}
					else if(CurrentTest->NetworkingTimeLimit.Result == EFunKFunctionalTestResult::Skipped)
					{
						RaiseInfoEvent(FString::Printf(TEXT("Netwoktimeout: %s"), *CurrentTest->NetworkingTimeLimit.Message.ToString()), GetName());
					}
					else
					{
						RaiseErrorEvent(FString::Printf(TEXT("Netwoktimeout=%s. %s"), *LexToString(CurrentTest->NetworkingTimeLimit.Result), *CurrentTest->NetworkingTimeLimit.Message.ToString()), GetName());
					}

					SubExecutionIds.Empty();
				}
			}
		}

		if(IsFinished())
		{
			FinishedCurrentTest();
		}
	}
	else
	{
		FinishedCurrentLocalTest();
		FinishedCurrentTest();
	}
	
	Super::Tick(DeltaTime);
}

void AFunKWorldTestController::ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> reportSink)
{
	for (TActorIterator<AFunKFunctionalTest> ActorItr(GetWorld(), AFunKFunctionalTest::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKFunctionalTest* FunctionalTest = *ActorItr;
		if(TestName == FunctionalTest->GetName())
		{
			ExecuteTest(FunctionalTest, reportSink, FGuid::NewGuid());
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> reportSink)
{
}

bool AFunKWorldTestController::IsFinished() const
{
	return IsLocallyFinished() && SubExecutionIds.IsEmpty();
}

bool AFunKWorldTestController::IsLocallyFinished() const
{
	return !CurrentTest || CurrentTest->IsFinished();
}

void AFunKWorldTestController::RaiseInfoEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent(EFunKEventType::Info, Message, Context));
}

void AFunKWorldTestController::RaiseWarningEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent(EFunKEventType::Warning, Message, Context));
}

void AFunKWorldTestController::RaiseErrorEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent(EFunKEventType::Error, Message, Context));
}

void AFunKWorldTestController::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(!ReportEvent(raisedEvent))
	{
		SendEvent(raisedEvent);
	}
}

int32 AFunKWorldTestController::GetActiveControllerCount() const
{
	return ActiveController;
}

bool AFunKWorldTestController::IsLocalTestController() const
{
	return GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->GetLocalTestController() == this;
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"

#include "EngineUtils.h"
#include "FunKEngineSubsystem.h"
#include "FunKFunctionalTest.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "FunKWorldSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Sinks/FunKLogSink.h"


// Sets default values
AFunKWorldTestController::AFunKWorldTestController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("TEST");
}

// Called when the game starts or when spawned
void AFunKWorldTestController::BeginPlay()
{
	Super::BeginPlay();

	if(GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
	{
		FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &AFunKWorldTestController::OnConnection);
		//auto it = GetWorld()->GetPlayerControllerIterator();
		//for(; it; ++it)
		//{
		//	if(it->IsValid() && !it->Get()->IsLocalController())
		//	{
		//		CreateTestControllerForClient(it->Get());
		//	}
		//}
	}

	SetActorTickEnabled(false);
}

void AFunKWorldTestController::CreateTestControllerForClient(APlayerController* NewPlayer)
{
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
		}
	}
}

void AFunKWorldTestController::ExecuteTest(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> reportSink)
{
	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Standalone)
	{
		ExecuteTestRun(TestToExecute, reportSink);
	}
	else if(netMode == NM_Client)
	{
		ReportSink = reportSink;
		ServerExecuteTest(TestToExecute);
	}
	else
	{
		ExecuteTestRun(TestToExecute, reportSink);

		for (AFunKWorldTestController* Controller : SpawnedController)
		{
			Controller->ReportSink = this;
			ClientExecuteTest(TestToExecute);
		}
	}
}

void AFunKWorldTestController::FinishedCurrentTest()
{
	SetActorTickEnabled(false);
	CurrentTest = nullptr;
	ReportSink = nullptr;
}

void AFunKWorldTestController::OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	CreateTestControllerForClient(NewPlayer);
}

void AFunKWorldTestController::ServerExecuteTest_Implementation(AFunKFunctionalTest* TestToExecute)
{
	if(TestToExecute)
	{
		if(UFunKWorldSubsystem* funkWorldSubsystem = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
		{
			funkWorldSubsystem->GetLocalTestController()->ExecuteTest(TestToExecute, this);
		}
		else
		{
			UE_LOG(FunKLog, Error, TEXT("When the server execute test is called it should always have an Creator..."));
		}
	}
	else
	{
		RaiseErrorEvent("Could not invoke test actor!", "AFunKWorldTestController::ServerExecuteTest_Implementation");
	}
}

void AFunKWorldTestController::ClientExecuteTest_Implementation(AFunKFunctionalTest* TestToExecute)
{
	if(TestToExecute)
	{
		ExecuteTestRun(TestToExecute, nullptr);
	}
	else
	{
		RaiseErrorEvent("Could not invoke test actor!", "AFunKWorldTestController::ClientExecuteTest_Implementation");
	}
}

void AFunKWorldTestController::ExecuteTestRun(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> reportSink)
{
	if(!ReportSink)
		ReportSink = reportSink;
	
	CurrentTest = TestToExecute;
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

void AFunKWorldTestController::ApplySendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	if(ReportSink)
	{
		ReportSink->RaiseEvent(FFunKEvent(eventType, Message, Context));
	}
	else
	{
		UFunKLogSink::LogEvent(FFunKEvent(eventType, Message, Context));
	}
}

void AFunKWorldTestController::ServerSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	ApplySendEvent(eventType, Message, Context);
}

void AFunKWorldTestController::ClientSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	ApplySendEvent(eventType, Message, Context);
}

// Called every frame
void AFunKWorldTestController::Tick(float DeltaTime)
{
	if(CurrentTest)
	{
		if(CurrentTest->GetTestResult() != EFunKFunctionalTestResult::None)
		{
			FinishedCurrentTest();
		}
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
			ExecuteTest(FunctionalTest, reportSink);
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests(TScriptInterface<IFunKSink> reportSink)
{
}

bool AFunKWorldTestController::IsFinished() const
{
	return !CurrentTest || (CurrentTest->IsStarted() && CurrentTest->IsFinished());
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
	if(ReportSink)
	{
		ReportSink->RaiseEvent(raisedEvent);
	}
	else
	{
		SendEvent(raisedEvent);
	}
}


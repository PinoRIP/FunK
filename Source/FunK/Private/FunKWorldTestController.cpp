// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"

#include "EngineUtils.h"
#include "FunKEngineSubsystem.h"
#include "FunKFunctionalTest.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "GameFramework/GameModeBase.h"


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
	}
	UFunKEngineSubsystem::FunKDebug("AFunKWorldTestController!");

	SetActorTickEnabled(false);
}

void AFunKWorldTestController::CreateTestControllerForClient(APlayerController* NewPlayer)
{
	AFunKWorldTestController* NewController = GetWorld()->SpawnActor<AFunKWorldTestController>(GetClass());
	NewController->SetOwner(NewPlayer);
	NewController->Creator = this;
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
		if(const UFunKEngineSubsystem* funk = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>())
		{
			funk->RegisterController(this);
		}
	}
}

void AFunKWorldTestController::ExecuteTest(AFunKFunctionalTest* TestToExecute)
{
	IsRunning = true;

	if(!Creator)
	{
		CurrentTest = TestToExecute;
		SetActorTickEnabled(true);
		TestToExecute->RunTest(this);
	}

	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Standalone)
		return;

	if(netMode == NM_Client)
	{
		ServerExecuteTest(TestToExecute);
	}
	else
	{
		ClientExecuteTest(TestToExecute);
	}
}

void AFunKWorldTestController::FinishedCurrentTest()
{
	SetActorTickEnabled(false);
	CurrentTest = nullptr;
	TestOrigin = nullptr;
	IsRunning = false;
}

void AFunKWorldTestController::OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	CreateTestControllerForClient(NewPlayer);
}

void AFunKWorldTestController::ServerExecuteTest_Implementation(AFunKFunctionalTest* TestToExecute)
{
	if(TestToExecute)
	{
		//TODO: Protect this
		for (AFunKWorldTestController* controller : Creator->SpawnedController)
		{
			if(controller != this)
			{
				controller->TestOrigin = this;
				controller->ExecuteTest(TestToExecute);
			}
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
		TestToExecute->RunTest(this);
	}
	else
	{
		RaiseErrorEvent("Could not invoke test actor!", "AFunKWorldTestController::ClientExecuteTest_Implementation");
	}
}

void AFunKWorldTestController::SendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	if(GetNetMode() == NM_Standalone)
	{
		UE_LOG(FunKLog, Error, TEXT("SendEvent event should never be reached when testing standalone!"))
	}
	
	if(GetNetMode() == NM_Client)
	{
		ServerSendEvent(eventType, Message, Context);
	}
	else
	{
		ClientSendEvent(eventType, Message, Context);
	}
}

void AFunKWorldTestController::ApplySendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	switch (eventType)
	{
	case EFunKEventType::Info: RaiseInfoEvent(Message, Context); break;
	case EFunKEventType::Warning: RaiseWarningEvent(Message, Context); break;
	case EFunKEventType::Error: RaiseErrorEvent(Message, Context); break;
	default: ;
	}
}

void AFunKWorldTestController::ServerSendEvent_Implementation(EFunKEventType eventType, const FString& Message, const FString& Context) const
{
	if(TestOrigin)
	{
		TestOrigin->SendEvent(eventType, Message, Context);
	}
	else
	{
		ApplySendEvent(eventType, Message, Context);
	}
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

void AFunKWorldTestController::SetTestRunner(UFunKTestRunner* testRunner)
{
	TestRunner = testRunner;
}

void AFunKWorldTestController::ExecuteTestByName(FString TestName)
{
	for (TActorIterator<AFunKFunctionalTest> ActorItr(GetWorld(), AFunKFunctionalTest::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKFunctionalTest* FunctionalTest = *ActorItr;
		if(TestName == FunctionalTest->GetName())
		{
			ExecuteTest(FunctionalTest);
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests()
{
	
}

bool AFunKWorldTestController::IsFinished()
{
	return !IsRunning;
}

void AFunKWorldTestController::RaiseInfoEvent(const FString& Message, const FString& Context) const
{
	if(TestRunner)
	{
		TestRunner->RaiseInfoEvent(Message, Context);
	}
}

void AFunKWorldTestController::RaiseWarningEvent(const FString& Message, const FString& Context) const
{
	if(TestRunner)
	{
		TestRunner->RaiseWarningEvent(Message, Context);
	}
}

void AFunKWorldTestController::RaiseErrorEvent(const FString& Message, const FString& Context) const
{
	if(TestRunner)
	{
		TestRunner->RaiseErrorEvent(Message, Context);
	}
}


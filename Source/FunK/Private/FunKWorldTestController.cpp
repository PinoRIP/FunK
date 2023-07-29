// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldTestController.h"
#include "EngineUtils.h"
#include "FunKFunctionalTest.h"
#include "FunKSettingsObject.h"
#include "FunKTestRunner.h"
#include "FunKWorldSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFunKWorldTestController::AFunKWorldTestController()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

// Called when the game starts or when spawned
void AFunKWorldTestController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	Settings = GetDefault<UFunKSettingsObject>();
}

void AFunKWorldTestController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!CurrentTest && Next())
	{
		CurrentTest->BeginTest(TestRunID, Seed);
	}

	if(!CurrentTest)
		return;
	
	if(!CurrentTest->IsRunning() && CurrentTest->IsFinished())
	{
		CurrentTest = nullptr;
	}
	else if(!IsWaitingForTimeout)
	{
		const FFunKStage* stage = CurrentTest->GetCurrentStage();
		if(stage && !stage->TimeLimit.IsLimitless() && stage->TimeLimit.IsTimeout(CurrentTest->GetCurrentStageExecutionTime()))
		{
			CurrentTest->FinishTest(stage->TimeLimit.Result, stage->TimeLimit.Message.ToString());
			IsWaitingForTimeout = true;
		}
	}
	else if(Settings)
	{
		CurrentTestStageWaitingTime += DeltaTime;
		if(Settings->Settings.SyncTimeLimit.IsTimeout(CurrentTestStageWaitingTime))
		{
			GetWorld()->GetSubsystem<UFunKEventBusSubsystem>()->Raise(FFunKEvent::Error("Reached Timeout error in test!", CurrentTest->GetName()));
			CurrentTest = nullptr;
		}
	}
	else
	{
		CurrentTest = nullptr;
	}
}

void AFunKWorldTestController::ExecuteTestByName(FString TestName)
{
	Tests.Empty(1);
	
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		if(TestName == FunctionalTest->GetName())
		{
			Tests.Add(FunctionalTest);
			ExecuteTests();
			break;
		}
	}
}

void AFunKWorldTestController::ExecuteAllTests()
{
	Tests.Empty();
	
	for (TActorIterator<AFunKTestBase> ActorItr(GetWorld(), AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		AFunKTestBase* FunctionalTest = *ActorItr;
		Tests.Add(FunctionalTest);
	}

	ExecuteTests();
}

bool AFunKWorldTestController::IsFinished() const
{
	return TestRunID == 0;
}

void AFunKWorldTestController::ExecuteTests()
{
	TestRunID = FMath::Rand();
	if(TestRunID == 0) TestRunID = 1;
	
	Seed = FMath::Rand();
	
	SetActorTickEnabled(true);
}

bool AFunKWorldTestController::Next()
{
	if(Tests.Num() <= 0)
	{
		End();
		return false;
	}

	IsWaitingForTimeout = false;
	CurrentTestStageWaitingTime = 0;
	const int32 last = Tests.Num() - 1;
	AFunKTestBase* NextTest = Tests[last];
	if(NextTest->IsRunning()) return false;
	
	CurrentTest = NextTest;
	Tests.RemoveAt(last);
	
	return true;
}

void AFunKWorldTestController::End()
{
	TestRunID = 0;
	SetActorTickEnabled(false);
}

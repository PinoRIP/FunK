// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"

#include "FunK.h"
#include "FunKEngineSubsystem.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"


// Sets default values
AFunKFunctionalTest::AFunKFunctionalTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFunKFunctionalTest::RunTest(AFunKWorldTestController* controller)
{
	Controller = controller;
	RunFrame = GFrameNumber;
	RunTime = GetWorld()->GetTimeSeconds();

	TotalTime = 0;
	PreparationTime = 0;
	ExecutionTime = 0;
	StartFrame = 0;
	StartTime = 0;

	IsSetupReady = false;

	if(!BpAssume())
	{
		FinishTest(EFunKFunctionalTestResult::Skipped, "Assumption not full filled");
		return;
	}

	TestResult = EFunKFunctionalTestResult::None;
	BpStartSetup();
}

void AFunKFunctionalTest::FinishTest(EFunKFunctionalTestResult testResult, const FString& Message)
{
	SetActorTickEnabled(false);
	TestResult = testResult;

	if(TestResult == EFunKFunctionalTestResult::Succeeded)
	{
		RaiseInfoEvent(FString::Printf(TEXT("Test Finished: %s"), *Message), GetName());
	}
	else if(TestResult == EFunKFunctionalTestResult::Skipped)
	{
		RaiseInfoEvent(FString::Printf(TEXT("Skipped: %s"), *Message), GetName());
	}
	else
	{
		RaiseErrorEvent(FString::Printf(TEXT("FinishTest TestResult=%s. %s"), *LexToString(TestResult), *Message), GetName());
	}

	BpCleanup();
	GEngine->ForceGarbageCollection();

	Controller = nullptr;
}

void AFunKFunctionalTest::RaiseInfoEvent(const FString& Message, const FString& Context) const
{
	if(Controller)
	{
		Controller->RaiseInfoEvent(Message, Context);
	}
}

void AFunKFunctionalTest::RaiseWarningEvent(const FString& Message, const FString& Context) const
{
	if(Controller)
	{
		Controller->RaiseInfoEvent(Message, Context);
	}
}

void AFunKFunctionalTest::RaiseErrorEvent(const FString& Message, const FString& Context) const
{
	if(Controller)
	{
		Controller->RaiseInfoEvent(Message, Context);
	}
}

bool AFunKFunctionalTest::IsStarted() const
{
	return StartFrame > 0;
}

EFunKFunctionalTestResult AFunKFunctionalTest::GetTestResult() const
{
	return TestResult;
}

// Called when the game starts or when spawned
void AFunKFunctionalTest::BeginPlay()
{
	Super::BeginPlay();

	if(GEngine)
	{
		if(UFunKWorldSubsystem* funk = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
		{
			funk->CheckLocalTestController();
		}
	}

	SetActorTickEnabled(false);
}

bool AFunKFunctionalTest::Assume()
{
	return true;
}

bool AFunKFunctionalTest::BpAssume_Implementation()
{
	return Assume();
}

void AFunKFunctionalTest::StartSetup()
{
}

bool AFunKFunctionalTest::IsReady()
{
	return true;
}

void AFunKFunctionalTest::StartTest()
{
}

void AFunKFunctionalTest::BpStartTest_Implementation()
{
	StartTest();
}

bool AFunKFunctionalTest::BpReady_Implementation()
{
	return IsReady();
}

void AFunKFunctionalTest::BpStartSetup_Implementation()
{
	StartSetup();
}

void AFunKFunctionalTest::Cleanup()
{
}

void AFunKFunctionalTest::BpCleanup_Implementation()
{
	Cleanup();
}

void AFunKFunctionalTest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(TestResult == EFunKFunctionalTestResult::Default)
	{
		FinishTest(EFunKFunctionalTestResult::Invalid, TEXT("Test was aborted"));
	}
	
	Super::EndPlay(EndPlayReason);
}

bool AFunKFunctionalTest::IsTimeout(const FFunKTimeLimit& limit, float time)
{
	return limit.Time <= time;
}

void AFunKFunctionalTest::OnTimeout(const FFunKTimeLimit& limit)
{
	FinishTest(limit.Result, limit.Message.ToString());
}

// Called every frame
void AFunKFunctionalTest::Tick(float DeltaTime)
{
	if(TestResult != EFunKFunctionalTestResult::None)
		return;

	GEngine->DelayGarbageCollection();
	
	TotalTime += DeltaTime;

	if(!IsSetupReady)
	{
		IsSetupReady = BpReady();

		if(IsSetupReady)
		{
			StartTest();
			StartFrame = GFrameNumber;
			StartTime = GetWorld()->GetTimeSeconds();
		}
		else
		{
			PreparationTime += DeltaTime;
			if(IsTimeout(PreparationTimeLimit, PreparationTime))
			{
				OnTimeout(PreparationTimeLimit);
				return;
			}
		}
	}
	else
	{
		ExecutionTime += DeltaTime;
		if(IsTimeout(PreparationTimeLimit, ExecutionTime))
		{
			OnTimeout(PreparationTimeLimit);
			return;
		}
	}
	
	Super::Tick(DeltaTime);
}

void AFunKFunctionalTest::BuildTestRegistry(FString& append) const
{
	// Only include enabled tests in the list of functional tests to run.
	if (IsEnabled && (RunInStandaloneMode || RunInDedicatedServerMode || RunInListenServerMode))
	{
		append.Append(GetActorLabel() + TEXT("|") +
			(RunInStandaloneMode ? FFunKModule::FunkStandaloneParameter : TEXT("")) +
			(RunInDedicatedServerMode ? FFunKModule::FunkListenParameter : TEXT("")) +
			(RunInListenServerMode ? FFunKModule::FunkDedicatedParameter : TEXT("")) +
			":" + GetName()
		);
		append.Append(TEXT(";"));
	}
}

void AFunKFunctionalTest::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	Super::GetAssetRegistryTags(OutTags);

	if(IsPackageExternal() && IsEnabled)
	{
		FString TestActor;
		BuildTestRegistry(TestActor);
		const FName TestCategory = IsEditorOnlyObject(this) ? FFunKModule::FunkEditorOnlyTestWorldTag : FFunKModule::FunkTestWorldTag;
		OutTags.Add(UObject::FAssetRegistryTag(TestCategory, TestActor, UObject::FAssetRegistryTag::TT_Hidden));
	}
}


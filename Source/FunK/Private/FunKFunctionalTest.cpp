// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"
#include "FunK.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "FunKWorldTestExecution.h"


bool FFunKTimeLimit::IsTimeout(float time) const
{
	return IsLimitless() ? false : Time <= time;
}

bool FFunKTimeLimit::IsLimitless() const
{
	return Time == 0;
}

// Sets default values
AFunKFunctionalTest::AFunKFunctionalTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFunKFunctionalTest::BeginTestSetup(AFunKWorldTestController* controller, FFunKTestId testId)
{
	Controller = controller;
	RunFrame = GFrameNumber;
	RunTime = GetWorld()->GetTimeSeconds();
	TestId = testId;

	TotalTime = 0;
	PreparationTime = 0;
	ExecutionTime = 0;
	StartFrame = 0;
	StartTime = 0;
	IsTestStarted = false;
	IsSetupReady = false;

	if(!BpAssume())
	{
		FinishTest(EFunKFunctionalTestResult::Skipped, "Assumption not full filled");
		return;
	}

	TestResult = EFunKFunctionalTestResult::None;
	BpStartSetup();
	SetActorTickEnabled(true);
}

void AFunKFunctionalTest::BeginTestExecution()
{
	check(IsSetupReady)
	
	IsTestStarted = true;
	StartFrame = GFrameNumber;
	StartTime = GetWorld()->GetTimeSeconds();

	BpStartTest();
}

void AFunKFunctionalTest::FinishTest(EFunKFunctionalTestResult testResult, const FString& Message)
{
	SetActorTickEnabled(false);
	TestResult = testResult;

	RaiseEvent(CreateEvent(testResult, Message).AddToContext(GetName()).AddToContext(UFunKWorldTestExecution::FunKTestLifeTimeTestFinishedEvent).AddToContext(TestId.ToString()));

	BpCleanup();
	GEngine->ForceGarbageCollection();

	Controller = nullptr;
	TestId = FGuid();
}

FFunKEvent AFunKFunctionalTest::CreateEvent(EFunKFunctionalTestResult testResult, const FString& Message)
{
	FString EventMessage = testResult == EFunKFunctionalTestResult::Succeeded
		? FString::Printf(TEXT("Test Finished: %s"), *Message)
		: testResult == EFunKFunctionalTestResult::Skipped
			? FString::Printf(TEXT("Test Skipped: %s"), *Message)
			: FString::Printf(TEXT("TestResult=%s. %s"), *LexToString(testResult), *Message);

	return FFunKEvent(testResult == EFunKFunctionalTestResult::Succeeded || testResult == EFunKFunctionalTestResult::Skipped ? EFunKEventType::Info : EFunKEventType::Error, EventMessage);
}

bool AFunKFunctionalTest::IsStarted() const
{
	return IsTestStarted;
}

bool AFunKFunctionalTest::IsFinished() const
{
	return TestResult != EFunKFunctionalTestResult::None;
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
			RaiseEvent(FFunKEvent::Info("Preparation finished", TestId.ToString()).AddToContext(UFunKWorldTestExecution::FunKTestLifeTimePreparationCompleteEvent).AddToContext(Controller->GetRoleName()));
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
			(RunInDedicatedServerMode ? FFunKModule::FunkDedicatedParameter : TEXT("")) +
			(RunInListenServerMode ? FFunKModule::FunkListenParameter : TEXT("")) +
			":" + GetName()
		);
		append.Append(TEXT(";"));
	}
}

void AFunKFunctionalTest::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(Controller)
	{
		Controller->RaiseEvent(raisedEvent);
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


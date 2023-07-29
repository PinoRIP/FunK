// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestBase.h"

#include "FunK.h"
#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"

bool FFunKTimeLimit::IsTimeout(float time) const
{
	return IsLimitless() ? false : Time <= time;
}

bool FFunKTimeLimit::IsLimitless() const
{
	return Time == 0;
}

AFunKTestBase::AFunKTestBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

bool AFunKTestBase::IsRunningInStandaloneMode() const
{
	return RunOnStandalone;
}

bool AFunKTestBase::IsRunningInDedicatedServerMode() const
{
	return RunOnDedicatedServer || RunOnDedicatedServerClients;
}

bool AFunKTestBase::IsRunningInListenServerMode() const
{
	return RunOnListenServer || RunOnListenServerClients;
}

bool AFunKTestBase::GetRunOnDedicatedServer() const
{
	return RunOnDedicatedServer;
}

bool AFunKTestBase::GetRunOnDedicatedServerClients() const
{
	return RunOnDedicatedServerClients;
}

bool AFunKTestBase::GetRunOnListenServer() const
{
	return RunOnListenServer;
}

bool AFunKTestBase::GetRunOnListenServerClients() const
{
	return RunOnListenServerClients;
}

void AFunKTestBase::BeginTestSetup(AFunKWorldTestController* Controller, FFunKTestID testId)
{
	CurrentController = Controller;
	TestID = testId;
	IsTestStarted = false;
	IsSetupReady = false;

	if(!InvokeAssume())
	{
		FinishTest(EFunKFunctionalTestResult::Skipped, "Assumption not full filled");
		return;
	}

	TestResult = EFunKFunctionalTestResult::None;
	InvokeStartSetup();
	SetActorTickEnabled(true);
}

void AFunKTestBase::BeginTestExecution()
{
	check(CurrentController)
	
	IsTestStarted = true;

	InvokeStartTest();
}

void AFunKTestBase::FinishTest(EFunKFunctionalTestResult InTestResult, const FString& Message)
{
	SetActorTickEnabled(false);
	IsTestStarted = false;
	TestResult = InTestResult;

	RaiseEvent(CreateEvent(TestResult, Message).AddToContext(UFunKWorldTestExecution::FunKTestLifeTimeTestFinishedEvent));

	InvokeCleanup();
	GEngine->ForceGarbageCollection();

	CurrentController = nullptr;
	TestID = FGuid();
}

FFunKEvent AFunKTestBase::CreateEvent(EFunKFunctionalTestResult testResult, const FString& Message) const
{
	FString EventMessage = testResult == EFunKFunctionalTestResult::Succeeded
		? FString::Printf(TEXT("Test Finished: %s"), *Message)
		: testResult == EFunKFunctionalTestResult::Skipped
			? FString::Printf(TEXT("Test Skipped: %s"), *Message)
			: FString::Printf(TEXT("TestResult=%s. %s"), *LexToString(testResult), *Message);

	return FFunKEvent(testResult == EFunKFunctionalTestResult::Succeeded || testResult == EFunKFunctionalTestResult::Skipped ? EFunKEventType::Info : EFunKEventType::Error, EventMessage, GetName());
}

bool AFunKTestBase::IsStarted() const
{
	return IsTestStarted;
}

bool AFunKTestBase::IsFinished() const
{
	return TestResult != EFunKFunctionalTestResult::None;
}

EFunKFunctionalTestResult AFunKTestBase::GetTestResult() const
{
	return TestResult;
}

void AFunKTestBase::BuildTestRegistry(FString& append) const
{
	// Only include enabled tests in the list of functional tests to run.
	if (IsEnabled && (IsRunningInStandaloneMode() || IsRunningInDedicatedServerMode() || IsRunningInListenServerMode()))
	{
		append.Append(GetActorLabel() + TEXT("|") +
			(IsRunningInStandaloneMode() ? FFunKModule::FunkStandaloneParameter : TEXT("")) +
			(IsRunningInDedicatedServerMode() ? FFunKModule::FunkDedicatedParameter : TEXT("")) +
			(IsRunningInListenServerMode() ? FFunKModule::FunkListenParameter : TEXT("")) +
			":" + GetName()
		);
		append.Append(TEXT(";"));
	}
}

void AFunKTestBase::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(CurrentController)
	{
		CurrentController->RaiseEvent(FFunKEvent(raisedEvent).AddToContext(TestID.ToString()).AddToContext(GetName()).AddToContext(CurrentController->GetRoleName()));
	}
}

FFunKTimeLimit* AFunKTestBase::GetPreparationTimeLimit()
{
	return nullptr;
}

FFunKTimeLimit* AFunKTestBase::GetTimeLimit()
{
	return nullptr;
}

FFunKTimeLimit* AFunKTestBase::GetNetworkingTimeLimit()
{
	return nullptr;
}

bool AFunKTestBase::InvokeAssume()
{
	return true;
}

void AFunKTestBase::InvokeStartSetup()
{
}

bool AFunKTestBase::InvokeIsReady()
{
	return true;
}

void AFunKTestBase::InvokeStartTest()
{
}

void AFunKTestBase::InvokeCleanup()
{
}

void AFunKTestBase::InvokeCheckForLocalTestController()
{
	if(UFunKWorldSubsystem* funk = GetWorld()->GetSubsystem<UFunKWorldSubsystem>())
	{
		funk->CheckLocalTestController();
	}
}

void AFunKTestBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(!IsFinished())
	{
		FinishTest(EFunKFunctionalTestResult::Invalid, TEXT("Test was aborted"));
	}
	
	Super::EndPlay(EndPlayReason);
}

void AFunKTestBase::BeginPlay()
{
	Super::BeginPlay();
	InvokeCheckForLocalTestController();
	SetActorTickEnabled(false);
}

void AFunKTestBase::Tick(float DeltaTime)
{
	if(IsFinished())
		return;

	GEngine->DelayGarbageCollection();

	if(!IsSetupReady)
	{
		IsSetupReady = InvokeIsReady();

		if(IsSetupReady)
		{
			RaiseEvent(FFunKEvent::Info("Preparation finished", UFunKWorldTestExecution::FunKTestLifeTimePreparationCompleteEvent));
		}
	}
	
	Super::Tick(DeltaTime);
}

void AFunKTestBase::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
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

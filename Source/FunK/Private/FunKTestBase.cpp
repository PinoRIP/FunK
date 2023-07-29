// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestBase.h"

#include "FunK.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Stages/FunKStagesSetup.h"

AFunKTestBase::AFunKTestBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

bool AFunKTestBase::IsStandaloneModeTest() const
{
	return Stages.OnStandaloneCount > 0;
}

bool AFunKTestBase::IsDedicatedServerModeTest() const
{
	return IsRunOnDedicatedServer() || IsRunOnDedicatedServerClients();
}

bool AFunKTestBase::IsListenServerModeTest() const
{
	return IsRunOnListenServer() || IsRunOnListenServerClients();
}

bool AFunKTestBase::IsRunOnDedicatedServer() const
{
	return Stages.OnListenServerCount > 0;
}

bool AFunKTestBase::IsRunOnDedicatedServerClients() const
{
	return Stages.OnListenServerClientCount > 0;
}

bool AFunKTestBase::IsRunOnListenServer() const
{
	return Stages.OnListenServerCount > 0;
}

bool AFunKTestBase::IsRunOnListenServerClients() const
{
	return Stages.OnListenServerClientCount > 0;
}

void AFunKTestBase::BeginTest(AFunKWorldTestController* Controller, FFunKTestID testId)
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

void AFunKTestBase::BeginTestStage()
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

	CleanupAfterTest();
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
	if (IsEnabled && (IsStandaloneModeTest() || IsDedicatedServerModeTest() || IsListenServerModeTest()))
	{
		append.Append(GetActorLabel() + TEXT("|") +
			(IsStandaloneModeTest() ? FFunKModule::FunkStandaloneParameter : TEXT("")) +
			(IsDedicatedServerModeTest() ? FFunKModule::FunkDedicatedParameter : TEXT("")) +
			(IsListenServerModeTest() ? FFunKModule::FunkListenParameter : TEXT("")) +
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

void AFunKTestBase::PostLoad()
{
	Super::PostLoad();
	SetupStages();
}

void AFunKTestBase::PostActorCreated()
{
	Super::PostActorCreated();
	SetupStages();
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

void AFunKTestBase::CleanupAfterTest()
{
}

void AFunKTestBase::CheckLocalTestController()
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
	CheckLocalTestController();
	SetActorTickEnabled(false);
}

void AFunKTestBase::SetupStages(FFunKStagesSetup& stages)
{
}

void AFunKTestBase::SetupStages()
{
	if(Stages.Stages.Num() > 0)
		return;
	
	FFunKStagesSetup stagesFluentSetup = FFunKStagesSetup(&Stages, this);
	SetupStages(stagesFluentSetup);
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

const FFunKStages* AFunKTestBase::GetStages() const
{
	return &Stages;
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

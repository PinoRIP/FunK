// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestBase.h"

#include "FunK.h"
#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "FunKWorldTestExecution.h"
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
	return Stages.OnDedicatedServerCount > 0;
}

bool AFunKTestBase::IsRunOnDedicatedServerClients() const
{
	return Stages.OnDedicatedServerClientCount > 0;
}

bool AFunKTestBase::IsRunOnListenServer() const
{
	return Stages.OnListenServerCount > 0;
}

bool AFunKTestBase::IsRunOnListenServerClients() const
{
	return Stages.OnListenServerClientCount > 0;
}

void AFunKTestBase::BeginTest(AFunKWorldTestController* Controller, FFunKTestRunID InTestRunID)
{
	CurrentController = Controller;
	TestRunID = InTestRunID;
	Result = EFunKTestResult::None;
	SetActorTickEnabled(true);
}

void AFunKTestBase::BeginTestStage(int32 StageIndex)
{
	if(!IsValidStageIndex(StageIndex))
	{
		FinishStage(EFunKTestResult::Error, "Test has no stage " + FString::FromInt(StageIndex));
		return;
	}

	CurrentStageIndex = StageIndex;
	IsCurrentStageTickDelegateSetup = IsStageTickDelegateBound(StageIndex);
	OnBeginStage(GetStageName());

	if(const FFunKStage* CurrentStage = GetCurrentStageMutable()) if(!CurrentStage->IsLatent)
	{
		FinishStage(EFunKTestResult::None, "");
	}
}

FName AFunKTestBase::GetStageName() const
{
	if(const FFunKStage* CurrentStage = GetCurrentStage())
		return CurrentStage->Name;

	return NAME_None;
}

void AFunKTestBase::FinishStage()
{
	FinishStage(EFunKTestResult::None, "");
}

void AFunKTestBase::FinishStage(EFunKTestResult TestResult, const FString& Message)
{
	FString ResultMessage = Message;
	if (TestResult == EFunKTestResult::None && CurrentStageIndex >= Stages.Stages.Num() - 1 && CurrentStageIndex != INDEX_NONE)
	{
		TestResult = EFunKTestResult::Invalid;
		ResultMessage = "Last stage didn't specify result! " + ResultMessage;
	}

	OnFinishStage(GetStageName());
	
	if(TestResult != EFunKTestResult::None)
	{
		SetActorTickEnabled(false);
		Result = TestResult;

		OnFinish(ResultMessage);
		GEngine->ForceGarbageCollection();

		CurrentController = nullptr;
		TestRunID = FGuid();
	}

	CurrentStageIndex = INDEX_NONE;
}

FFunKEvent AFunKTestBase::CreateEvent(EFunKTestResult testResult, const FString& Message) const
{
	const FString EventMessage = testResult == EFunKTestResult::Succeeded
         ? FString::Printf(TEXT("Test Finished: %s"), *Message)
         : testResult == EFunKTestResult::Skipped
         ? FString::Printf(TEXT("Test Skipped: %s"), *Message)
         : FString::Printf(TEXT("TestResult=%s. %s"), *LexToString(testResult), *Message);

	return FFunKEvent(testResult == EFunKTestResult::Succeeded || testResult == EFunKTestResult::Skipped ? EFunKEventType::Info : EFunKEventType::Error, EventMessage, GetName());
}

bool AFunKTestBase::IsRunning() const
{
	return TestRunID.IsValid();
}

bool AFunKTestBase::IsStageRunning() const
{
	return CurrentStageIndex > INDEX_NONE;
}

bool AFunKTestBase::IsFinished() const
{
	return Result != EFunKTestResult::None;
}

EFunKTestResult AFunKTestBase::GetTestResult() const
{
	return Result;
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
		FFunKEvent Event(raisedEvent);
		
		Event.AddToContext(TestRunID.ToString()).AddToContext(GetName()).AddToContext(CurrentController->GetRoleName());
		if(const FFunKStage* CurrentStage = GetCurrentStage())
			Event.AddToContext(CurrentStage->Name.ToString());
		
		CurrentController->RaiseEvent(Event);
	}
	else
	{
		const FString Type = raisedEvent.Type == EFunKEventType::Info ? "Info" : raisedEvent.Type == EFunKEventType::Warning ? "Warning" : "Error";
		UE_LOG(FunKLog, Error, TEXT("Event could not be raised - %s: %s - %s"), *Type, *raisedEvent.Message, *raisedEvent.GetContext())
	}
}

FFunKTimeLimit* AFunKTestBase::GetSyncTimeLimit()
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

void AFunKTestBase::OnBegin()
{
	RaiseEvent(FFunKEvent::Info("Begin Test " + GetName(), UFunKWorldTestExecution::FunKTestLifeTimeBeginEvent));
}

void AFunKTestBase::OnBeginStage(const FName& StageName)
{
	RaiseEvent(FFunKEvent::Info("Begin Stage " + StageName.ToString(), UFunKWorldTestExecution::FunKTestLifeTimeBeginStageEvent));

	if(const FFunKStage* CurrentStage = GetCurrentStageMutable())
	{
		if(!CurrentStage->StartDelegate.ExecuteIfBound())
		{
			FinishStage(EFunKTestResult::Error, "Start delegate not found!");
		}
	}
}

void AFunKTestBase::OnFinishStage(const FName& StageName)
{
	RaiseEvent(FFunKEvent::Info("Finish Stage " + StageName.ToString(), UFunKWorldTestExecution::FunKTestLifeTimeFinishStageEvent));
}

void AFunKTestBase::OnFinish(const FString& Message)
{
	RaiseEvent(CreateEvent(Result, Message).AddToContext(UFunKWorldTestExecution::FunKTestLifeTimeFinishEvent));
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
	if(IsRunning() && !IsFinished())
	{
		FinishStage(EFunKTestResult::Invalid, TEXT("Test was aborted"));
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

const FFunKStage* AFunKTestBase::GetCurrentStage() const
{
	return GetStage(CurrentStageIndex);
}

const FFunKStage* AFunKTestBase::GetStage(int32 StageIndex) const
{
	if(IsValidStageIndex(StageIndex))
		return &Stages.Stages[StageIndex];

	return nullptr;
}

bool AFunKTestBase::IsStageTickDelegateBound(int32 StageIndex)
{
	return IsValidStageIndex(StageIndex) && Stages.Stages[StageIndex].TickDelegate.IsBound();
}

void AFunKTestBase::SetupStages()
{
	if(Stages.Stages.Num() > 0)
		return;
	
	FFunKStagesSetup stagesFluentSetup = FFunKStagesSetup(&Stages, this);
	SetupStages(stagesFluentSetup);
}

bool AFunKTestBase::IsValidStageIndex(int32 StageIndex) const
{
	return Stages.Stages.Num() > StageIndex && StageIndex >= 0;
}

FFunKStage* AFunKTestBase::GetCurrentStageMutable()
{
	return GetStageMutable(CurrentStageIndex);
}

FFunKStage* AFunKTestBase::GetStageMutable(int32 StageIndex)
{
	if(IsValidStageIndex(StageIndex))
		return &Stages.Stages[StageIndex];

	return nullptr;
}

void AFunKTestBase::Tick(float DeltaTime)
{
	if(IsFinished())
		return;

	GEngine->DelayGarbageCollection();

	if(IsCurrentStageTickDelegateSetup) if(const FFunKStage* CurrentStage = GetCurrentStageMutable())
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		CurrentStage->TickDelegate.ExecuteIfBound(DeltaTime);
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

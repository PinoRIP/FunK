// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestBase.h"
#include "FunK.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "FunKWorldTestExecution.h"
#include "Events/FunKEvent.h"
#include "Events/Internal/FunKTestFinishedEvent.h"
#include "Events/Internal/FunKTestStageEvent.h"
#include "Events/Internal/FunKTestStageFinishedEvent.h"
#include "Stages/FunKStagesSetup.h"
#include "Util/FunKAnonymousBitmask.h"

struct FFunKAnonymousBitmask;
struct FFunKTestFinishedEvent;

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

void AFunKTestBase::BeginTest(FGuid InTestRunID, int32 InSeed)
{
	if(IsRunning()) return;
	
	PeerBitMask = FFunKAnonymousBitmask(GetWorldSubsystem()->GetPeerCount());
	RunningRegistrations.Add(GetEventBusSubsystem()->On<FFunKTestStageFinishedEvent>([this](const FFunKTestStageFinishedEvent& Event)
	{
		if(Event.Test == this)
		{
			OnFinishStage(Event);
		}
	}));
	
	NextStage(InTestRunID, InSeed);
}

void AFunKTestBase::OnFinishStage(const FFunKTestStageFinishedEvent& Event)
{
	if(Event.StageIndex != CurrentStageIndex) return;
	if(Event.TestRunID != TestRunID) return;

	PeerBitMask.Set(Event.PeerIndex);
	if(PeerBitMask.IsSet())
	{
		PeerBitMask.ClearAll();
		NextStage(Event.TestRunID, GetSeed());
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
	FinishStage(EFunKStageResult::Succeeded, "");
}

void AFunKTestBase::FinishStage(EFunKStageResult TestResult, const FString& Message)
{
	FString ResultMessage = Message;
	if (TestResult == EFunKStageResult::None)
	{
		TestResult = EFunKStageResult::Error;
		ResultMessage = "None is an invalid stage result! " + ResultMessage;
	}

	OnFinishStage(TestResult, Message);

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
	// if(CurrentController)
	// {
	// 	FFunKEvent Event(raisedEvent);
	// 	
	// 	Event.AddToContext(TestRunID.ToString()).AddToContext(GetName()).AddToContext(FString::FromInt(GetWorldSubsystem()->GetPeerIndex()));
	// 	if(const FFunKStage* CurrentStage = GetCurrentStage())
	// 		Event.AddToContext(CurrentStage->Name.ToString());
	// 	
	// 	CurrentController->RaiseEvent(Event);
	// }
	// else
	// {
	// 	const FString Type = raisedEvent.Type == EFunKEventType::Info ? "Info" : raisedEvent.Type == EFunKEventType::Warning ? "Warning" : "Error";
	// 	UE_LOG(FunKLog, Error, TEXT("Event could not be raised - %s: %s - %s"), *Type, *raisedEvent.Message, *raisedEvent.GetContext())
	// }
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

int32 AFunKTestBase::GetSeed() const
{
	return Seed;
}

void AFunKTestBase::OnBegin()
{
	RaiseEvent(FFunKEvent::Info("Begin Test " + GetName(), UFunKWorldTestExecution::FunKTestLifeTimeBeginEvent));
}

void AFunKTestBase::OnBeginStage()
{
	if(const FFunKStage* CurrentStage = GetCurrentStageMutable())
	{
		if(!CurrentStage->StartDelegate.ExecuteIfBound())
		{
			FinishStage(EFunKStageResult::Error, "Start delegate not found!");
		}
	}
}

void AFunKTestBase::OnFinishStage(EFunKStageResult StageResult, FString Message)
{
	FFunKTestStageFinishedEvent Event;
	Event.StageIndex = CurrentStageIndex;
	Event.Result = StageResult;
	Event.Message = Message;
	Event.PeerIndex = GetWorldSubsystem()->GetPeerIndex();
	Event.TestRunID = TestRunID;
	Event.Test = this;
	
	GetEventBusSubsystem()->Raise<FFunKTestStageFinishedEvent>(Event);
}

void AFunKTestBase::OnFinish(const FString& Message)
{
}

bool AFunKTestBase::IsLastStage()
{
	ENetMode netMode = GetNetMode();
	const int32 index = GetCurrentStageIndex() + 1;
	if(!IsValidStageIndex(index))
		return true;

	const TArray<FFunKStage>& stages = GetStages()->Stages;
	for (int32 i = index; i < stages.Num(); ++i)
	{
		if (IsExecutingStage(stages[i]))
			return false;
	}

	return true;
}

bool AFunKTestBase::IsExecutingStage(const FFunKStage& stage) const
{
	ENetMode netMode = GetNetMode();
	return 	(stage.IsOnStandalone && netMode == NM_Standalone) ||
			(stage.IsOnDedicatedServer && netMode == NM_DedicatedServer) ||
			(stage.IsOnListenServer && netMode == NM_ListenServer) ||
			(netMode == NM_Client && (GetWorldSubsystem()->IsServerDedicated() ? stage.IsOnDedicatedServerClient : stage.IsOnListenServerClient));
}

void AFunKTestBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsRunning() && !IsFinished())
	{
		// FinishStage(EFunKTestResult::Invalid, TEXT("Test was aborted"));
	}
	
	Super::EndPlay(EndPlayReason);
}

void AFunKTestBase::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	BeginRegistration = GetEventBusSubsystem()->On<FFunKTestStageEvent>([this](const FFunKTestStageEvent& Event)
	{
		if(Event.Test == this)
		{
			OnBeginStage(Event);
		}
	});
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

void AFunKTestBase::OnBeginStage(const FFunKTestStageEvent& Event)
{
	if(!IsValidStageIndex(Event.StageIndex))
	{
		FinishStage(EFunKStageResult::Error, "Test has no stage " + FString::FromInt(Event.StageIndex));
		return;
	}

	if(Event.StageIndex != CurrentStageIndex + 1)
	{
		FinishStage(EFunKStageResult::Error, "Test cant skip stages! " + FString::FromInt(CurrentStageIndex) + " - " + FString::FromInt(Event.StageIndex));
		return;
	}
	
	if(!IsRunning())
	{
		OnBeginFirstStage(Event);
	}
	else if(TestRunID != Event.TestRunID)
	{
		FinishStage(EFunKStageResult::Error, "Test is running under different test id: " + TestRunID.ToString() + " - " + Event.TestRunID.ToString());
		return;
	}

	UpdateStageState(Event);
	IsCurrentStageTickDelegateSetup = IsStageTickDelegateBound(Event.StageIndex);
	OnBeginStage();

	if(const FFunKStage* CurrentStage = GetCurrentStageMutable()) if(!CurrentStage->IsLatent)
	{
		FinishStage(EFunKStageResult::Succeeded, "");
	}
}

void AFunKTestBase::UpdateStageState(const FFunKTestStageEvent& Event)
{
	Seed = Event.Seed;
	CurrentStageIndex = Event.StageIndex;
}

void AFunKTestBase::OnBeginFirstStage(const FFunKTestStageEvent& Event)
{
	SetActorTickEnabled(true);
	TestRunID = Event.TestRunID;
	Result = EFunKTestResult::None;

	RunningRegistrations.Add(GetEventBusSubsystem()->On<FFunKTestFinishedEvent>([this](const FFunKTestFinishedEvent& FinishedEvent)
	{
		if(FinishedEvent.Test == this)
		{
			OnFinish(FinishedEvent);
		}
	}));

	UpdateStageState(Event);
	OnBegin();
}

void AFunKTestBase::OnFinish(const FFunKTestFinishedEvent& Event)
{
	SetActorTickEnabled(false);
	Result = Event.Result;

	OnFinish(Event.Message);
	GEngine->ForceGarbageCollection();

	RunningRegistrations.Unregister();
	CurrentStageIndex = INDEX_NONE;
	TestRunID = FGuid();
	Seed = 0;
}

void AFunKTestBase::NextStage(FGuid InTestRunID, int32 InSeed)
{
	FFunKTestStageEvent TestStageEvent;
	
	TestStageEvent.Seed = InSeed;
	TestStageEvent.TestRunID = InTestRunID;
	GetEventBusSubsystem()->Raise(TestStageEvent);
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

UFunKWorldSubsystem* AFunKTestBase::GetWorldSubsystem() const
{
	return GetWorld()->GetSubsystem<UFunKWorldSubsystem>();
}

UFunKEventBusSubsystem* AFunKTestBase::GetEventBusSubsystem() const
{
	return GetWorld()->GetSubsystem<UFunKEventBusSubsystem>();
}

bool AFunKTestBase::IsBpEventImplemented(const FName& Name) const
{
	const UFunction* function = FindFunction(Name);
	return function && function->IsInBlueprint();
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

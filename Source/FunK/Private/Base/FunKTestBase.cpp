// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/FunKTestBase.h"
#include "FunK.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Events/FunKEvent.h"
#include "Events/FunKTestLifeTimeContext.h"
#include "Events/Internal/FunKTestFinishedEvent.h"
#include "Events/Internal/FunKTestStageFinishedEvent.h"
#include "Base/FunKStagesSetup.h"
#include "Events/Internal/FunKTestBeginEvent.h"
#include "Events/Internal/FunKTestFinishEvent.h"
#include "Net/UnrealNetwork.h"
#include "Util/FunKAnonymousBitmask.h"

struct FFunKAnonymousBitmask;
struct FFunKTestFinishedEvent;

AFunKTestBase::AFunKTestBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
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

void AFunKTestBase::BeginTest(int32 InTestRunID, int32 InSeed)
{
	if (IsRunning())
	{
		return;
	}

	if (!IsDriver())
	{
		FFunKTestBeginEvent TestBeginEvent;
		TestBeginEvent.Seed = InSeed;
		TestBeginEvent.TestRunID = InTestRunID;
		TestBeginEvent.Test = this;

		GetEventBusSubsystem()->Raise(TestBeginEvent);
	}
	else
	{
		PeerBitMask = FFunKAnonymousBitmask(GetWorldSubsystem()->GetPeerCount());
		RunningRegistrations.Add(GetEventBusSubsystem()->On<FFunKTestStageFinishedEvent>([this](const FFunKTestStageFinishedEvent& Event)
		{
			if (Event.Test == this)
			{
				OnFinishStage(Event);
			}
		}));
		
		RunningRegistrations.Add(GetEventBusSubsystem()->On<FFunKTestFinishEvent>([this](const FFunKTestFinishEvent& Event)
		{
			if (Event.Test == this)
			{
				Finish(Event.Result, Event.Message);
			}
		}));

		RaiseEvent(FFunKEvent::Info("Start Test", FunKTestLifeTimeContext::BeginTest).AddToContext(InTestRunID));

		NextStage(InTestRunID, InSeed);
	}
}

void AFunKTestBase::NextStage(int32 InTestRunID, int32 InSeed)
{
	const int32 NextStageIndex = GetNextStageIndex();
	if (!IsValidStageIndex(NextStageIndex))
	{
		Finish(EFunKTestResult::Succeeded, "");
		return;
	}

	BeginStage(InTestRunID, InSeed, NextStageIndex);
}

void AFunKTestBase::BeginStage(int32 InTestRunID, int32 InSeed, int32 StageIndex)
{
	if (IsDriver())
	{
		if (!IsValidStageIndex(StageIndex))
		{
			FinishStage(EFunKStageResult::Error, "Test has no stage " + FString::FromInt(StageIndex));
			return;
		}

		if(IsServer())
			ClientBeginStage(InTestRunID, InSeed, StageIndex);
	}

	if (!IsRunning())
	{
		OnBeginFirstStage(InTestRunID, InSeed, StageIndex);
	}
	else if (TestRunID != InTestRunID)
	{
		FinishStage(EFunKStageResult::Error, "Test is running under different test id: " + FString::FromInt(TestRunID) + " - " + FString::FromInt(InTestRunID));
		return;
	}

	UpdateStageState(InSeed, StageIndex);

	if (IsDriver())
	{
		RaiseEvent(FFunKEvent::Info("Start Stage", FunKTestLifeTimeContext::BeginStage).Ref());
	}

	const FFunKStage* CurrentStage = GetCurrentStageMutable();
	if (!CurrentStage || !IsExecutingStage(*CurrentStage))
	{
		FinishStage();
		return;
	}

	IsCurrentStageTickDelegateSetup = CurrentStage->TickDelegate.IsBound();
	OnBeginStage();

	if (!CurrentStage->IsLatent && !IsLocalStageFinished)
	{
		FinishStage();
	}
}

void AFunKTestBase::ClientBeginStage_Implementation(int32 InTestRunID, int32 InSeed, int32 StageIndex)
{
	if(IsDriver()) return;
	
	BeginStage(InTestRunID, InSeed, StageIndex);
}

void AFunKTestBase::OnBeginFirstStage(int32 InTestRunID, int32 InSeed, int32 StageIndex)
{
	SetActorTickEnabled(true);
	TestRunID = InTestRunID;
	Result = EFunKTestResult::None;

	RunningRegistrations.Add(GetEventBusSubsystem()->On<FFunKTestFinishedEvent>([this](const FFunKTestFinishedEvent& FinishedEvent)
	{
		if (FinishedEvent.Test == this && FinishedEvent.TestRunID == TestRunID)
		{
			OnFinish(FinishedEvent);
		}
	}));

	UpdateStageState(InSeed, StageIndex);
	OnBegin();
}

void AFunKTestBase::UpdateStageState(int32 InSeed, int32 StageIndex)
{
	Seed = InSeed;
	CurrentStageIndex = StageIndex;
	IsLocalStageFinished = false;
	CurrentStageExecutionTime = 0;
}

void AFunKTestBase::OnBegin()
{
}

bool AFunKTestBase::IsExecutingStage(const FFunKStage& stage) const
{
	ENetMode netMode = GetNetMode();
	return (stage.IsOnStandalone && netMode == NM_Standalone) ||
		(stage.IsOnDedicatedServer && netMode == NM_DedicatedServer) ||
		(stage.IsOnListenServer && netMode == NM_ListenServer) ||
		(netMode == NM_Client && (GetWorldSubsystem()->IsServerDedicated() ? stage.IsOnDedicatedServerClient : stage.IsOnListenServerClient));
}

void AFunKTestBase::OnBeginStage()
{
	if (const FFunKStage* CurrentStage = GetCurrentStageMutable())
	{
		if (!CurrentStage->StartDelegate.ExecuteIfBound())
		{
			FinishStage(EFunKStageResult::Error, "Start delegate not found!");
		}
	}
}

void AFunKTestBase::FinishTest(const FString& Reason)
{
	FinishTest(EFunKTestResult::Invalid, Reason);
}

void AFunKTestBase::FinishTest(EFunKTestResult InResult, const FString& Reason)
{
	if (!IsRunning())
	{
		return;
	}

	Finish(InResult, Reason.Len() > 0 ? Reason : "Test was manually finished");
}

void AFunKTestBase::Finish(EFunKTestResult TestResult, FString Message)
{
	if (IsDriver())
	{
		FFunKTestFinishedEvent Event;
		Event.TestRunID = TestRunID;
		Event.Test = this;
		Event.Result = TestResult;
		Event.Message = Message;

		GetEventBusSubsystem()->Raise(Event, [this]()
		{
			IsFinishComplete = true;
		});
	}
	else
	{
		FFunKTestFinishEvent Event;
		Event.TestRunID = TestRunID;
		Event.Test = this;
		Event.Result = TestResult;
		Event.Message = Message;

		GetEventBusSubsystem()->Raise(Event);
	}
}

void AFunKTestBase::OnFinish(const FFunKTestFinishedEvent& Event)
{
	SetActorTickEnabled(false);
	Result = Event.Result;

	OnFinish(Event.Message);
	GEngine->ForceGarbageCollection();

	if (IsDriver())
	{
		const FString EventMessage = Event.Result == EFunKTestResult::Succeeded
										 ? FString::Printf(TEXT("Test Finished: %s"), *Event.Message)
										 : Event.Result == EFunKTestResult::Skipped
										 ? FString::Printf(TEXT("Test Skipped: %s"), *Event.Message)
										 : FString::Printf(TEXT("TestResult=%s. %s"), *LexToString(Event.Result), *Event.Message);

		RaiseEvent(FFunKEvent(Event.Result == EFunKTestResult::Succeeded || Event.Result == EFunKTestResult::Skipped ? EFunKEventType::Info : EFunKEventType::Error, EventMessage, FunKTestLifeTimeContext::FinishTest).AddToContext(LexToString(Event.Result)));
	}

	UpdateStageState(0, INDEX_NONE);
	RunningRegistrations.Unregister();
	TestRunID = 0;
}

void AFunKTestBase::OnFinish(const FString& Message)
{
}

void AFunKTestBase::FinishStage()
{
	FinishStage(EFunKStageResult::Succeeded, "");
}

void AFunKTestBase::FinishStage(EFunKStageResult StageResult, const FString& Message)
{
	if (Result != EFunKTestResult::None)
	{
		return;
	}

	IsLocalStageFinished = true;

	FString ResultMessage = Message;
	if (StageResult == EFunKStageResult::None)
	{
		StageResult = EFunKStageResult::Error;
		ResultMessage = "None is an invalid stage result! " + ResultMessage;
	}

	OnFinishStage(StageResult, Message);
}

void AFunKTestBase::OnFinishStage(EFunKStageResult StageResult, FString Message)
{
	FFunKTestStageFinishedEvent Event;
	Event.StageIndex = GetCurrentStageIndex();
	Event.Result = StageResult;
	Event.Message = Message;
	Event.PeerIndex = GetWorldSubsystem()->GetPeerIndex();
	Event.TestRunID = TestRunID;
	Event.Test = this;

	GetEventBusSubsystem()->Raise<FFunKTestStageFinishedEvent>(Event);
}

void AFunKTestBase::OnFinishStage(const FFunKTestStageFinishedEvent& Event)
{
	if (Event.StageIndex != CurrentStageIndex)
	{
		return;
	}
	if (Event.TestRunID != TestRunID)
	{
		return;
	}

	if (Event.Result != EFunKStageResult::Succeeded)
	{
		RaiseEvent(FFunKEvent::Info("Finish Stage", FunKTestLifeTimeContext::FinishStage).AddToContext(LexToString(Event.Result)));
		Finish(StageToTestResult(Event.Result), Event.Message);
		return;
	}

	PeerBitMask.Set(Event.PeerIndex);
	if (PeerBitMask.IsSet())
	{
		auto a = FFunKEvent::Info("Finish Stage", FunKTestLifeTimeContext::FinishStage);
		RaiseEvent(a);
		PeerBitMask.ClearAll();
		NextStage(TestRunID, GetSeed());
	}
}

bool AFunKTestBase::IsRunning() const
{
	return TestRunID != 0;
}

bool AFunKTestBase::IsFinished() const
{
	return Result != EFunKTestResult::None && IsFinishComplete;
}

FName AFunKTestBase::GetStageName() const
{
	if (const FFunKStage* CurrentStage = GetCurrentStage())
	{
		return CurrentStage->Name;
	}

	return NAME_None;
}

const FFunKStage* AFunKTestBase::GetCurrentStage() const
{
	return GetStage(CurrentStageIndex);
}

bool AFunKTestBase::IsBpEventImplemented(const FName& Name) const
{
	const UFunction* function = FindFunction(Name);
	return function && function->IsInBlueprint();
}

void AFunKTestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFunKTestBase, IsFinishComplete);
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

void AFunKTestBase::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	if (!IsDriver())
	{
		return;
	}

	BeginRegistration = GetEventBusSubsystem()->On<FFunKTestBeginEvent>([this](const FFunKTestBeginEvent& Event)
	{
		if (Event.Test == this)
		{
			BeginTest(Event.TestRunID, Event.Seed);
		}
	});
}

void AFunKTestBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsRunning() && !IsFinished())
	{
		FinishStage(EFunKStageResult::Error, TEXT("Test was aborted"));
	}

	Super::EndPlay(EndPlayReason);
}

void AFunKTestBase::SetupStages(FFunKStagesSetup& stages)
{
}

const FFunKStage* AFunKTestBase::GetStage(int32 StageIndex) const
{
	if (IsValidStageIndex(StageIndex))
	{
		return &Stages.Stages[StageIndex];
	}

	return nullptr;
}

bool AFunKTestBase::IsStageTickDelegateBound(int32 StageIndex)
{
	return IsValidStageIndex(StageIndex) && Stages.Stages[StageIndex].TickDelegate.IsBound();
}

int32 AFunKTestBase::GetNextStageIndex() const
{
	const ENetMode NetMode = GetNetMode();

	bool IsStandalone = NetMode == NM_Standalone;
	bool IsDedicatedServer = NetMode == NM_DedicatedServer;
	bool IsListenServer = NetMode == NM_ListenServer;
	if (NetMode == NM_Client)
	{
		IsDedicatedServer = GetWorldSubsystem()->IsServerDedicated();
		IsListenServer = !IsDedicatedServer;
	}

	const TArray<FFunKStage>& stages = GetStages()->Stages;
	for (int i = CurrentStageIndex + 1; i < stages.Num(); ++i)
	{
		if (
			stages[i].IsOnStandalone && IsStandalone ||
			((stages[i].IsOnDedicatedServer || stages[i].IsOnDedicatedServerClient) && IsDedicatedServer) ||
			((stages[i].IsOnListenServer || stages[i].IsOnListenServerClient) && IsListenServer)
		)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool AFunKTestBase::IsServer() const
{
	const ENetMode NetMode = GetNetMode();
	return NetMode != NM_Standalone && NetMode < NM_Client;
}

bool AFunKTestBase::IsDriver() const
{
	const ENetMode NetMode = GetNetMode();
	return NetMode < NM_Client;
}

void AFunKTestBase::SetupStages()
{
	if (Stages.Stages.Num() > 0)
	{
		return;
	}

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

void AFunKTestBase::GatherContext(FFunKEvent& Event) const
{
	if (TestRunID != 0)
	{
		Event.AddToContext(TestRunID);
	}

	if (CurrentStageIndex > INDEX_NONE)
	{
		Event.AddToContext(GetStageName().ToString());
	}

	Event.AddToContext(GetName());
}

FFunKStage* AFunKTestBase::GetCurrentStageMutable()
{
	return GetStageMutable(CurrentStageIndex);
}

FFunKStage* AFunKTestBase::GetStageMutable(int32 StageIndex)
{
	if (IsValidStageIndex(StageIndex))
	{
		return &Stages.Stages[StageIndex];
	}

	return nullptr;
}

void AFunKTestBase::Tick(float DeltaTime)
{
	if (IsFinished())
	{
		return;
	}

	CurrentStageExecutionTime += DeltaTime;

	GEngine->DelayGarbageCollection();

	if (IsLocalStageFinished)
	{
		return;
	}

	if (IsCurrentStageTickDelegateSetup)
	{
		if (const FFunKStage* CurrentStage = GetCurrentStageMutable())
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			CurrentStage->TickDelegate.ExecuteIfBound(DeltaTime);
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

	if (IsPackageExternal() && IsEnabled)
	{
		FString TestActor;
		BuildTestRegistry(TestActor);
		const FName TestCategory = IsEditorOnlyObject(this) ? FFunKModule::FunkEditorOnlyTestWorldTag : FFunKModule::FunkTestWorldTag;
		OutTags.Add(FAssetRegistryTag(TestCategory, TestActor, FAssetRegistryTag::TT_Hidden));
	}
}

void AFunKTestBase::RaiseEvent(FFunKEvent& Event) const
{
	GatherContext(Event);
	GetEventBusSubsystem()->Raise<FFunKEvent>(Event);
}

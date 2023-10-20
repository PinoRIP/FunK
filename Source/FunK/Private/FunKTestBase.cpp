// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestBase.h"
#include "FunK.h"
#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DebugCameraController.h"
#include "Extensions/FunKTestFragment.h"
#include "InputSimulation/FunKInputSimulationSystem.h"
#include "Events/FunKTestEvents.h"
#include "Events/FunKTestLifeTimeContext.h"
#include "Setup/FunKStagesSetup.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Extensions/FunKTestRootVariationComponent.h"
#include "Extensions/FunKTestVariationComponent.h"

int32 FFunKTestVariations::GetCount() const
{
	auto variations = FMath::Max(1, GetVariationsCount());
	auto rootVariations = FMath::Max(1, GetRootVariationsCount());
	
	return variations * rootVariations;
}

int32 FFunKTestVariations::GetVariationsCount() const
{
	int32 Count = 0;
	for (UFunKTestVariationComponent* Variation : Variations)
	{
		Count += Variation->GetCount();
	}

	return Count;
}

int32 FFunKTestVariations::GetRootVariationsCount() const
{
	return RootVariations ? RootVariations->GetCount() : 1;
}

AFunKTestBase::AFunKTestBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;
	
	auto SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		SpriteComponent->bHiddenInGame = true;
#if WITH_EDITORONLY_DATA

		if (!IsRunningCommandlet())
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> Texture(TEXT("/Engine/EditorResources/S_FTest"));
			SpriteComponent->Sprite = Texture.Get();
			SpriteComponent->SpriteInfo.Category = TEXT("FunKTests");
			SpriteComponent->SpriteInfo.DisplayName = FText::FromName(TEXT("FunKTests"));
		}

#endif
	}
	
	RootComponent = SpriteComponent;

#if WITH_EDITORONLY_DATA
	TestName = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("TestName"));
	if(TestName)
	{
		TestName->bHiddenInGame = true;
		TestName->SetHorizontalAlignment(EHTA_Center);
		TestName->SetRelativeLocation(FVector(0, 0, 80));
		TestName->SetRelativeRotation(FRotator(0, 0, 0));
		TestName->SetupAttachment(RootComponent);
	}
#endif
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

void AFunKTestBase::BeginTest(int32 InTestRunID, int32 InSeed, int32 Variation)
{
	if (IsRunning())
	{
		return;
	}

	if (!IsDriver())
	{
		FFunKTestRequestBeginEvent TestBeginEvent;
		TestBeginEvent.Seed = InSeed;
		TestBeginEvent.TestRunID = InTestRunID;
		TestBeginEvent.Test = this;
		TestBeginEvent.Variation = Variation;

		GetEventBusSubsystem()->Raise(TestBeginEvent);
	}
	else
	{
		FFunKTestBeginEvent TestBeginEvent;
		TestBeginEvent.Seed = InSeed;
		TestBeginEvent.TestRunID = InTestRunID;
		TestBeginEvent.Variation = Variation;
		TestBeginEvent.Test = this;
		
		GetEventBusSubsystem()->Raise(TestBeginEvent);
	}
}

void AFunKTestBase::NextStage()
{
	const int32 NextStageIndex = GetNextStageIndex();
	if (!IsValidStageIndex(NextStageIndex))
	{
		Finish(EFunKTestResult::Succeeded, "");
		return;
	}

	FFunKTestStageBeginEvent StageBeginEvent;
	StageBeginEvent.Stage = NextStageIndex;
	StageBeginEvent.TestRunID = TestRunID;
	StageBeginEvent.Test = this;
	
	GetEventBusSubsystem()->Raise(StageBeginEvent);
}

bool AFunKTestBase::IsDriver(ENetMode NetMode)
{
	return NetMode < NM_Client;
}

FFunKEvent& AFunKTestBase::FillEnvironmentContext(FFunKEvent& Event) const
{
	Event.AddToContext(GetName());
	Event.AddToContext(GetPeerIndex());

	return Event;
}

void AFunKTestBase::DispatchRaisedEvent(const FFunKEvent& Event) const
{
	GetEventBusSubsystem()->Raise<FFunKEvent>(Event);
}

void AFunKTestBase::OnBegin(const FFunKTestBeginEvent& BeginEvent)
{
	if(IsRunning())
	{
		Error("Test is already running", FString::FromInt(BeginEvent.TestRunID));
		return;
	}
	
	TestRunID = BeginEvent.TestRunID;
	Seed = BeginEvent.Seed;
	SetCurrentVariation(BeginEvent.Variation);
	Result = EFunKTestResult::None;
	PeerBitMask = FFunKAnonymousBitmask(GetWorldSubsystem()->GetPeerCount());
	CurrentStageIndex = INDEX_NONE;
	SetActorTickEnabled(true);

	if(GetNetMode() != NM_DedicatedServer)
	{
		UFunKInputSimulationSystem* InputSimulationSystem = GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
		InputSimulationSystem->DisableActualInputs();

		ViewObservationPoint();
	}
	
	if(IsDriver())
	{
		RaiseEvent(FFunKEvent::Info("Start Test", FunKTestLifeTimeContext::BeginTest).Ref());
		NextStage();
	}
}

bool AFunKTestBase::IsExecutingStage(const FFunKStage& stage) const
{
	ENetMode netMode = GetNetMode();
	return (stage.IsOnStandalone && netMode == NM_Standalone) ||
		(stage.IsOnDedicatedServer && netMode == NM_DedicatedServer) ||
		(stage.IsOnListenServer && netMode == NM_ListenServer) ||
		(netMode == NM_Client && (GetWorldSubsystem()->IsServerDedicated() ? stage.IsOnDedicatedServerClient : stage.IsOnListenServerClient));
}

void AFunKTestBase::OnBeginStage(const FFunKTestStageBeginEvent& BeginEvent)
{
	PeerBitMask.ClearAll();
	CurrentStageIndex = BeginEvent.Stage;
	IsLocalStageFinished = false;
	CurrentStageExecutionTime = 0;

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

	OnBeginStageFragments();
	IsCurrentStageTickDelegateSetup = CurrentStage->TickDelegate.IsBound();
	OnInvokeStage();
	
	if (!CurrentStage->IsLatent && !IsLocalStageFinished)
	{
		FinishStage();
	}
}

void AFunKTestBase::OnInvokeStage()
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
		FFunKTestFinishEvent Event;
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
		FFunKTestRequestFinishEvent Event;
		Event.TestRunID = TestRunID;
		Event.Test = this;
		Event.Result = TestResult;
		Event.Message = Message;

		GetEventBusSubsystem()->Raise(Event);
	}
}

void AFunKTestBase::OnFinish(const FFunKTestFinishEvent& Event)
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

	if(GetNetMode() != NM_DedicatedServer)
	{
		UFunKInputSimulationSystem* InputSimulationSystem = GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
		InputSimulationSystem->EnableActualInputs();
	}
	
	TestRunID = 0;
	Seed = 0;
	CurrentStageIndex = INDEX_NONE;
	SetCurrentVariation(INDEX_NONE);
	IsVariationBegun = false;
}

void AFunKTestBase::OnFinish(const FString& Message)
{
	OnTestFinish.Broadcast();

	ClearFragments(TestFragments);

	EndAllInputSimulations();
}

void AFunKTestBase::OnNetworkedFunctionalitiesReceived(const FFunKTestNetworkedFunctionalitiesCreatedEvent& Event)
{
	if (Event.TestRunID != TestRunID || Event.Stage != CurrentStageIndex)
	{
		return;
	}

	if (TestFragments.Num() != Event.Functionalities.Num())
	{
		Error("Fragment mismatch!");
		return;
	}

	const FFunKTestVariations& TestVariations = GetTestVariations();
	for (int i = 0; i < Event.Functionalities.Num(); ++i)
	{
		if(Event.Functionalities[i])
		{
			AddTestFragment(Event.Functionalities[i], i);

			if(i == 0)
			{
				if (TestVariations.RootVariations)
					TestVariations.RootVariations->OnUsing(Event.Functionalities[i]);
				else if(CurrentVariationComponent)
					CurrentVariationComponent->OnUsing(Event.Functionalities[i]);
			}
			else if (i == 1)
			{
				if(TestVariations.RootVariations && CurrentVariationComponent)
					CurrentVariationComponent->OnUsing(Event.Functionalities[i]);
			}
		}
	}
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
	OnFinishStageFragments();
	ClearFragments(StageFragments);
	
	FFunKTestStageFinishEvent Event;
	Event.Stage = GetCurrentStageIndex();
	Event.Result = StageResult;
	Event.Message = Message;
	Event.PeerIndex = GetPeerIndex();
	Event.TestRunID = TestRunID;
	Event.Test = this;

	GetEventBusSubsystem()->Raise<FFunKTestStageFinishEvent>(Event);
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

void AFunKTestBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if ( TestName )
	{
		TestName->SetText(FText::FromString(GetActorLabel()));
		
		if ( IsEnabled )
		{
			TestName->SetTextRenderColor(FColor(45, 255, 0));
		}
		else
		{
			TestName->SetTextRenderColor(FColor(55, 55, 55));
		}
	}
#endif
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
	stages.AddNamedLatentStage<AFunKTestBase>("ArrangeVariation", &AFunKTestBase::ArrangeVariation)
		.WithTickDelegate<AFunKTestBase>(&AFunKTestBase::ArrangeVariationTick)
		.UpdateTimeLimit(ArrangeVariationTimeLimit);
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

void AFunKTestBase::OnFinishStage(const FFunKTestStageFinishEvent& StageFinishEvent)
{
	if(!IsRunning()) return;
	
	if (StageFinishEvent.TestRunID != TestRunID)
	{
		Error("Stage finish event received from invalid test id", "Invalid id: " + FString::FromInt(StageFinishEvent.TestRunID));
		return;
	}
	
	if (StageFinishEvent.Stage != CurrentStageIndex)
	{
		Error("Stage finish event received from invalid stage", "Event stage: " + FString::FromInt(StageFinishEvent.Stage));
		return;
	}

	PeerBitMask.Set(StageFinishEvent.PeerIndex);
	OnPeerStageFinishing.Broadcast(StageFinishEvent.PeerIndex);

	if(!IsDriver())
		return;
	
	if (StageFinishEvent.Result != EFunKStageResult::Succeeded)
	{
		RaiseEvent(FFunKEvent::Info("Finish Stage", FunKTestLifeTimeContext::FinishStage).AddToContext(LexToString(StageFinishEvent.Result)));
		Finish(StageToTestResult(StageFinishEvent.Result), StageFinishEvent.Message);
		return;
	}

	if (PeerBitMask.IsSet())
	{
		RaiseEvent(FFunKEvent::Info("Finish Stage", FunKTestLifeTimeContext::FinishStage).Ref());
		PeerBitMask.ClearAll();
		NextStage();
	}
}

bool AFunKTestBase::IsServer() const
{
	const ENetMode NetMode = GetNetMode();
	return NetMode != NM_Standalone && NetMode < NM_Client;
}

bool AFunKTestBase::IsDriver() const
{
	return IsDriver(GetNetMode());
}

void AFunKTestBase::Info(const FString& Message, const FString& Context) const
{
	DispatchRaisedEvent(FillEnvironmentContext(FFunKEvent::Info(Message, Context).Ref()));
}

void AFunKTestBase::Warning(const FString& Message, const FString& Context) const
{
	DispatchRaisedEvent(FillEnvironmentContext(FFunKEvent::Warning(Message, Context).Ref()));
}

void AFunKTestBase::Error(const FString& Message, const FString& Context) const
{
	DispatchRaisedEvent(FillEnvironmentContext(FFunKEvent::Error(Message, Context).Ref()));
}

UFunKTestFragment* AFunKTestBase::AddTestFragment(UFunKTestFragment* Fragment)
{
	if(Fragment->IsSupportedForNetworking())
	{
		UE_LOG(FunKLog, Warning, TEXT("Directly added test fragments don't support networked functionalities! (yet?)"))
	}
	
	AddTestFragment(Fragment, INDEX_NONE);
	return Fragment;
}

UFunKTestFragment* AFunKTestBase::AddStageFragment(UFunKTestFragment* Fragment)
{
	if(Fragment->IsSupportedForNetworking())
	{
		UE_LOG(FunKLog, Warning, TEXT("Directly added test fragments don't support networked functionalities! (yet?)"))
	}
	
	AddStageFragment(Fragment, INDEX_NONE);
	return Fragment;
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

const FFunKAnonymousBitmask& AFunKTestBase::GetStagePeerState() const
{
	return PeerBitMask;
}

int32 AFunKTestBase::GetPeerIndex() const
{
	return GetWorldSubsystem()->GetPeerIndex();
}

void AFunKTestBase::RegisterEvents(UFunKEventBusSubsystem* EventBusSubsystem)
{
	if(IsDriver(EventBusSubsystem->GetWorld()->GetNetMode()))
	{
		EventBusSubsystem->On<FFunKTestRequestBeginEvent>([](const FFunKTestRequestBeginEvent& Event)
		{
			Event.Test->BeginTest(Event.TestRunID, Event.Seed, Event.Variation);
		});

		EventBusSubsystem->On<FFunKTestRequestFinishEvent>([](const FFunKTestRequestFinishEvent& Event)
		{
			Event.Test->Finish(Event.Result, Event.Message);
		});
	}
	else
	{
		EventBusSubsystem->On<FFunKTestNetworkedFunctionalitiesCreatedEvent>([](const FFunKTestNetworkedFunctionalitiesCreatedEvent& Event)
		{
			Event.Test->OnNetworkedFunctionalitiesReceived(Event);
		});
	}
	
	EventBusSubsystem->On<FFunKTestBeginEvent>([](const FFunKTestBeginEvent& Event)
	{
		Event.Test->OnBegin(Event);
	});

	EventBusSubsystem->On<FFunKTestFinishEvent>([](const FFunKTestFinishEvent& FinishedEvent)
	{
		FinishedEvent.Test->OnFinish(FinishedEvent);
	});
		
	EventBusSubsystem->On<FFunKTestStageBeginEvent>([](const FFunKTestStageBeginEvent& Event)
	{
		Event.Test->OnBeginStage(Event);
	});
		
	EventBusSubsystem->On<FFunKTestStageFinishEvent>([](const FFunKTestStageFinishEvent& Event)
	{
		Event.Test->OnFinishStage(Event);
	});
}

const FFunKTestVariations& AFunKTestBase::GetTestVariations()
{
	if(Variations.IsGathered) return Variations;
	Variations = BuildTestVariations();
	return Variations;
}

const FFunKTestVariations& AFunKTestBase::GetTestVariationsConst() const
{
	//TODO: just don't do this :)
	return const_cast<AFunKTestBase*>(this)->GetTestVariations();
}

int32 AFunKTestBase::GetTestVariationCount()
{
	const FFunKTestVariations& TestVariations = GetTestVariations();
	return TestVariations.GetCount();
}

FFunKTestVariation AFunKTestBase::GetCurrentVariation()
{	
	return FFunKTestVariation(
		CurrentVariationComponent,
		GetVariationComponentFragment(CurrentVariationComponent),
		GetVariationComponentFragmentIndex(CurrentVariationComponent),
		Variations.RootVariations,
		GetVariationComponentFragment(Variations.RootVariations),
		GetVariationComponentFragmentIndex(Variations.RootVariations)
	);
}

void AFunKTestBase::EndAllInputSimulations() const
{
	UFunKInputSimulationSystem* InputSimulationSystem = GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
	InputSimulationSystem->EndAllInputSimulations();
}

void AFunKTestBase::ArrangeVariation()
{
	IsVariationBegun = true;
	
	const FFunKTestVariations& TestVariations = GetTestVariations();
	AddVariationComponentFragment(TestVariations.RootVariations);
	AddVariationComponentFragment(CurrentVariationComponent);
}

void AFunKTestBase::ArrangeVariationTick(float DeltaTime)
{
	const FFunKTestVariations& TestVariations = GetTestVariations();
	if(IsVariationComponentReady(TestVariations.RootVariations, CurrentRootVariation) && IsVariationComponentReady(CurrentVariationComponent, CurrentVariation))
	{
		if(GetNetMode() != NM_DedicatedServer)
			ViewObservationPoint();
		
		FinishStage();
	}
}

void AFunKTestBase::ViewObservationPoint() const
{
	if (ObservationPoint == nullptr) return;

	const UWorld* World = GetWorld();
	if (World && World->GetGameInstance())
	{
		for (FConstPlayerControllerIterator PCIterator = World->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
		{
			APlayerController* PlayerController = PCIterator->Get();

			if (PlayerController && !PlayerController->IsA(ADebugCameraController::StaticClass()))
			{
				PlayerController->SetViewTarget(ObservationPoint);
				return;
			}
		}
	}
}

void AFunKTestBase::OnBeginStageFragments()
{
	for (UFunKTestFragment* Fragment : TestFragments)
	{
		Fragment->OnBeginStage();
	}
}

void AFunKTestBase::OnFinishStageFragments()
{
	for (UFunKTestFragment* Fragment : TestFragments)
	{
		Fragment->OnFinishStage();
	}
	
	for (UFunKTestFragment* Fragment : StageFragments)
	{
		Fragment->OnFinishStage();
	}
}

void AFunKTestBase::AddVariationComponentFragment(UFunKTestVariationComponent* VariationComponent)
{
	if(VariationComponent == nullptr) return;
	
	UFunKTestFragment* Fragment = VariationComponent->GetFragment(CurrentRootVariation);
	if(Fragment->IsSupportedForNetworking() && GetNetMode() == NM_Client)
	{
		TestFragments.Add(nullptr);
	}
	else
	{
		AddTestFragment(Fragment, INDEX_NONE);
		VariationComponent->OnUsing(Fragment);
	}
}

int32 AFunKTestBase::GetVariationComponentFragmentIndex(const UFunKTestVariationComponent* VariationComponent) const
{
	if (VariationComponent == nullptr) return INDEX_NONE;
	const FFunKTestVariations& TestVariations = GetTestVariationsConst();
	return TestVariations.RootVariations == nullptr || TestVariations.RootVariations == VariationComponent ? 0 : 1;	
}

FString AFunKTestBase::GetVariationComponentFragmentName(const UFunKTestVariationComponent* VariationComponent) const
{
	const UFunKTestFragment* Fragment = GetVariationComponentFragment(VariationComponent);
	return Fragment ? Fragment->GetReadableIdent() : "";
}

UFunKTestFragment* AFunKTestBase::GetVariationComponentFragment(const UFunKTestVariationComponent* VariationComponent) const
{
	if(TestFragments.Num() <= 0) return nullptr;
	
	const int32 Index = GetVariationComponentFragmentIndex(VariationComponent);

	if (TestFragments.Num() <= Index) return nullptr;
	return TestFragments[Index];
}

bool AFunKTestBase::IsVariationComponentReady(UFunKTestVariationComponent* VariationComponent, int32 Index) const
{
	if (!VariationComponent) return true;
	
	const int32 FragmentIndex = GetVariationComponentFragmentIndex(VariationComponent);
	UFunKTestFragment* Fragment = TestFragments[FragmentIndex];
	
	return VariationComponent->IsReady(Fragment, Index);
}

void AFunKTestBase::AddTestFragment(UFunKTestFragment* Fragment, int32 Index)
{
	AddFragment(TestFragments, Fragment, Index);
}

void AFunKTestBase::AddStageFragment(UFunKTestFragment* Fragment, int32 Index)
{
	AddFragment(StageFragments, Fragment, Index);
	Fragment->OnBeginStage();
}

void AFunKTestBase::AddFragment(TArray<UFunKTestFragment*>& Fragments, UFunKTestFragment* Fragment, int32 Index)
{
	if(Index == INDEX_NONE)
		Fragments.Add(Fragment);
	else
		Fragments[Index] = Fragment;

	Fragment->Test = this;
	Fragment->OnAdded();
}

void AFunKTestBase::ClearFragments(TArray<UFunKTestFragment*>& Fragments)
{
	for (UFunKTestFragment* Fragment : Fragments)
	{
		Fragment->OnRemoved();
		Fragment->Test = nullptr;
	}
	
	Fragments.Empty();
}

FFunKTestVariations AFunKTestBase::BuildTestVariations() const
{
	FFunKTestVariations NewVariations = FFunKTestVariations();
	NewVariations.IsGathered = true;
	const FFunKWorldVariations WorldVariations = GetWorldSubsystem()->GetWorldVariations();
	
	NewVariations.Variations = WorldVariations.Variations;
	
	TArray<UFunKTestVariationComponent*> Array;
	GetComponents<UFunKTestVariationComponent>(Array);
		
	Array.Sort([](const UFunKTestVariationComponent& ip1, const UFunKTestVariationComponent& ip2) {
		return  ip1.GetFName().FastLess(ip2.GetFName());
	});

	for (UFunKTestVariationComponent* FunKTestVariationComponent : Array)
	{
		if(FunKTestVariationComponent->IsA(UFunKTestRootVariationComponent::StaticClass()))
		{
			if(NewVariations.RootVariations)
			{
				UE_LOG(FunKLog, Error, TEXT("Only one root variation component is allowed per test! %s"), *GetName())
				continue;
			}
			
			NewVariations.RootVariations = Cast<UFunKTestRootVariationComponent>(FunKTestVariationComponent);
			continue;
		}
				
		NewVariations.Variations.Add(FunKTestVariationComponent);
	}

	return NewVariations;
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

	if(IsVariationBegun)
	{
		if(Variations.RootVariations && CurrentRootVariation != INDEX_NONE)
		{
			Event.AddToContext(Variations.RootVariations->GetName() + GetVariationComponentFragmentName(Variations.RootVariations));
		}

		if(CurrentVariationComponent && CurrentVariation != INDEX_NONE)
		{
			Event.AddToContext(CurrentVariationComponent->GetName() + GetVariationComponentFragmentName(CurrentVariationComponent));
		}
	}

	FillEnvironmentContext(Event);
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

void AFunKTestBase::SetCurrentVariation(int32 Variation)
{
	CurrentRootVariation = INDEX_NONE;
	CurrentVariation = INDEX_NONE;
	CurrentVariationComponent = nullptr;
	
	if (Variation != INDEX_NONE)
	{
		const FFunKTestVariations& TestVariations = GetTestVariations();
		CurrentRootVariation = Variation % TestVariations.GetRootVariationsCount();
		CurrentVariation = (Variation / TestVariations.GetRootVariationsCount());

		for (UFunKTestVariationComponent* VariationComponent : TestVariations.Variations)
		{
			const int32 variationCount = VariationComponent->GetCount();
			if(variationCount > CurrentVariation)
			{
				CurrentVariationComponent = VariationComponent;
				return;
			}
			CurrentVariation = CurrentVariation - variationCount;
		}
	}
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
	DispatchRaisedEvent(Event);
}

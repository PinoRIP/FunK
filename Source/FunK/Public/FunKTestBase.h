// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Events/FunKTestEvents.h"
#include "FunKTestResult.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Setup/FunKStages.h"
#include "Events/FunKEvent.h"
#include "Util/FunKAnonymousBitmask.h"
#include "FunKTestBase.generated.h"

struct FFunKStagesSetup;
class UFunKTestFragment;
class UFunKTestRootVariationComponent;
class UFunKTestVariationComponent;
class UFunKWorldSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFunKTestFinishing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFunKPeerStageFinishing, int32, PeerIndex);

USTRUCT()
struct FFunKTestVariations
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UFunKTestRootVariationComponent* RootVariations = nullptr;
	
	UPROPERTY()
	TArray<UFunKTestVariationComponent*> Variations;

	UPROPERTY()
	bool IsGathered = false;

	int32 GetCount() const;
	int32 GetVariationsCount() const;
	int32 GetRootVariationsCount() const;
};

USTRUCT(BlueprintType)
struct FFunKTestVariation
{
	GENERATED_BODY()

public:
	FFunKTestVariation()
		: FFunKTestVariation(nullptr, nullptr, INDEX_NONE, nullptr, nullptr, INDEX_NONE)
	{}
	
	FFunKTestVariation(UFunKTestVariationComponent* InVariation, UFunKTestFragment* InFragment, int32 InIndex, UFunKTestRootVariationComponent* InRoot, UFunKTestFragment* InRootFragment, int32 InRootIndex)
		: Variation(InVariation)
		, Fragment(InFragment)
		, Index(InIndex)
		, Root(InRoot)
		, RootFragment(InRootFragment)
		, RootIndex(InRootIndex)
	{
	}
	
	UPROPERTY(BlueprintReadOnly)
	UFunKTestVariationComponent* Variation;

	UPROPERTY(BlueprintReadOnly)
	UFunKTestFragment* Fragment;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Index;
	
	UPROPERTY(BlueprintReadOnly)
	UFunKTestRootVariationComponent* Root;

	UPROPERTY(BlueprintReadOnly)
	UFunKTestFragment* RootFragment;
	
	UPROPERTY(BlueprintReadOnly)
	int32 RootIndex;
};

/**
 * Base class for any kind of test
 */
UCLASS(Abstract, NotBlueprintable)
class FUNK_API AFunKTestBase : public AActor
{
	GENERATED_BODY()

public:
	AFunKTestBase();

	// Will the test run for standalone
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsStandaloneModeTest() const;

	// Will the test run for dedicated server 
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsDedicatedServerModeTest() const;

	// Will the test run for listen server
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsListenServerModeTest() const;

	// Is any stage active on the dedicated server
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnDedicatedServer() const;

	// Is any stage active on the client of dedicated servers
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnDedicatedServerClients() const;

	// Is any stage active on the listen server
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnListenServer() const;

	// Is any stage active on the client of listen servers
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnListenServerClients() const;

	// Begins this test
	virtual void BeginTest(int32 InTestRunID, int32 InSeed, int32 Variation);

	// Ends this test
	virtual void FinishTest(const FString& Reason = "");

	// Ends this test with a specific result
	virtual void FinishTest(EFunKTestResult InResult, const FString& Reason = "");

	// Is the test currently running
	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsRunning() const;

	// Is the test finished running
	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	// Gets the last result of this test
	UFUNCTION(BlueprintCallable, Category="FunK")
	EFunKTestResult GetTestResult() const { return Result; }

	// Gets the current seed for this test (Is the same on all peers)
	UFUNCTION(BlueprintCallable, Category="FunK")
	int32 GetSeed() const { return Seed; }

	// Gets the name of the current stage
	UFUNCTION(BlueprintCallable, Category="FunK")
	FORCEINLINE FName GetStageName() const;

	// Gets the current stage
	const FFunKStage* GetCurrentStage() const;

	// Gets the current stage index
	int32 GetCurrentStageIndex() const { return CurrentStageIndex; }

	// Gets all stages
	const FFunKStages* GetStages() const;

	// Gets the execution time of the current stage
	int32 GetCurrentStageExecutionTime() const { return CurrentStageExecutionTime; }

	// Ends the current stage
	virtual void FinishStage();

	// Ends the current stage with a specific result
	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishStage(EFunKStageResult StageResult, const FString& Message);

	// Gets the FunK world subsystem
	UFunKWorldSubsystem* GetWorldSubsystem() const;

	// Gets the eventbus subsystem
	UFunKEventBusSubsystem* GetEventBusSubsystem() const;

	// Event when the test is finished
	FFunKTestFinishing OnTestFinish;

	// Event when the stage is finished
	FFunKPeerStageFinishing OnPeerStageFinishing;

	// Gets a bitmask that indicates which peers have & have not finished the current stage
	const FFunKAnonymousBitmask& GetStagePeerState() const;

	// Gets the local peer index
	UFUNCTION(BlueprintCallable, Category="FunK")
	int32 GetPeerIndex() const;

	// Registers all events for tests in the event bus subsystem
	static void RegisterEvents(UFunKEventBusSubsystem* EventBusSubsystem);

	// Gets all test variations for the test
	const FFunKTestVariations& GetTestVariations();

	// Gets all test variations for the test as const (can be less performant)
	const FFunKTestVariations& GetTestVariationsConst() const;

	// Gets the amount of test variations needed for a full test completion
	int32 GetTestVariationCount();

	// Gets the current variation state
	UFUNCTION(BlueprintCallable)
	FFunKTestVariation GetCurrentVariation();

	// Raises a event for this test
	virtual void RaiseEvent(FFunKEvent& Event) const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout", AdvancedDisplay)
	FFunKTimeLimit ArrangeVariationTimeLimit;
	
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FunK", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsEnabled = true;

	// Called when the test begins
	virtual void OnBegin(const FFunKTestBeginEvent& BeginEvent);

	// Called when the stage begins
	virtual void OnBeginStage(const FFunKTestStageBeginEvent& BeginEvent);

	// Called when the actual stage function is called
	virtual void OnInvokeStage();

	// Called when the stage ends
	virtual void OnFinishStage(EFunKStageResult StageResult, FString Message);

	// Called when the test ends
	virtual void OnFinish(const FString& Message);

	// Called to configure the stages
	virtual void SetupStages(FFunKStagesSetup& StageSetup);

	// Gets a specific stage
	const FFunKStage* GetStage(int32 StageIndex) const;

	// Checks if the stage would be executed locally
	virtual bool IsExecutingStage(const FFunKStage& Stage) const;

	// Checks if there is a tick delegate bound for this stage
	bool IsStageTickDelegateBound(int32 StageIndex);

	// Checks if the index is valid for a stage
	bool IsValidStageIndex(int32 StageIndex) const;

	// Called to gather the context informations that will be send with every event of this test
	virtual void GatherContext(FFunKEvent& Event) const;

	// Is this locally on a server
	bool IsServer() const;

	// Is this locally the driving test (the test that synchronizes all peers)
	bool IsDriver() const;

	// Raises an info event
	UFUNCTION(BlueprintCallable, Category="FunK")
	void Info(const FString& Message, const FString& Context = "") const;

	// Raises a warning event
	UFUNCTION(BlueprintCallable, Category="FunK")
	void Warning(const FString& Message, const FString& Context = "") const;

	// Raises an error event
	UFUNCTION(BlueprintCallable, Category="FunK")
	void Error(const FString& Message, const FString& Context = "") const;

	// Adds a fragment to this test
	UFUNCTION(BlueprintCallable, Category="FunK")
	UFunKTestFragment* AddTestFragment(UFunKTestFragment* Fragment);

	// Adds a fragment to the current stage of this test
	UFUNCTION(BlueprintCallable, Category="FunK")
	UFunKTestFragment* AddStageFragment(UFunKTestFragment* Fragment);

private:
	int32 TestRunID;
	int32 Seed;
	int32 CurrentStageIndex = INDEX_NONE;
	FFunKStages Stages;
	EFunKTestResult Result = EFunKTestResult::None;
	FFunKAnonymousBitmask PeerBitMask;
	int32 CurrentVariation = INDEX_NONE;
	int32 CurrentRootVariation = INDEX_NONE;

	UPROPERTY(Transient)
	TArray<UFunKTestFragment*> TestFragments;

	UPROPERTY(Transient)
	TArray<UFunKTestFragment*> StageFragments;
	
	UPROPERTY(Transient)
	UFunKTestVariationComponent* CurrentVariationComponent = nullptr;

	UPROPERTY(Transient)
	FFunKTestVariations Variations;

	UPROPERTY(EditAnywhere, Category="FunK|Setup")
	TObjectPtr<AActor> ObservationPoint;

	bool IsVariationBegun = false;
	bool IsLocalStageFinished = false;
	bool IsCurrentStageTickDelegateSetup = false;
	float CurrentStageExecutionTime = 0;

	static bool IsDriver(ENetMode NetMode);
	
	UPROPERTY( replicated, Transient )
	bool IsFinishComplete;

	FFunKEvent& FillEnvironmentContext(FFunKEvent& Event) const;
	void DispatchRaisedEvent(const FFunKEvent& Event) const;
	
	void OnFinish(const FFunKTestFinishEvent& Event);
	void NextStage();
	void Finish(EFunKTestResult TestResult, FString Message);
	int32 GetNextStageIndex() const;

	void OnFinishStage(const FFunKTestStageFinishEvent& StageFinishEvent);
	
	void SetupStages();
	FFunKStage* GetCurrentStageMutable();
	FFunKStage* GetStageMutable(int32 StageIndex);

	void SetCurrentVariation(int32 Variation);

	void EndAllInputSimulations() const;

	UFUNCTION()
	void ArrangeVariation();

	UFUNCTION()
	void ArrangeVariationTick(float DeltaTime);

	void ViewObservationPoint() const;
	
	virtual void OnNetworkedFragmentsReceived(const FFunKTestNetworkedFragmentsCreatedEvent& Event);
	void OnBeginStageFragments();
	void OnFinishStageFragments();
	void AddVariationComponentFragment(UFunKTestVariationComponent* VariationComponent);
	int32 GetVariationComponentFragmentIndex(const UFunKTestVariationComponent* VariationComponent) const;
	FString GetVariationComponentFragmentName(const UFunKTestVariationComponent* VariationComponent) const;
	UFunKTestFragment* GetVariationComponentFragment(const UFunKTestVariationComponent* VariationComponent) const;
	bool IsVariationComponentReady(UFunKTestVariationComponent* VariationComponent, int32 Index) const;
	void AddTestFragment(UFunKTestFragment* Fragment, int32 Index);
	void AddStageFragment(UFunKTestFragment* Fragment, int32 Index);
	void AddFragment(TArray<UFunKTestFragment*>& Fragments, UFunKTestFragment* Fragment, int32 Index = INDEX_NONE);
	static void ClearFragments(TArray<UFunKTestFragment*>& Fragments);

	FFunKTestVariations BuildTestVariations() const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	virtual void OnConstruction(const FTransform& Transform) override;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR

	void BuildTestRegistry(FString& Append) const;
	bool IsBpEventImplemented(const FName& Name) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UTextRenderComponent> TestName;
#endif
		
	friend class UFunKAssertionBlueprintFunctionLibrary;
	friend class UFunKFragmentBlueprintFunctionLibrary;
};

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

class UFunKTestFragment;
class UFunKTestRootVariationComponent;
class UFunKTestVariationComponent;
struct FFunKStagesSetup;
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
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class FUNK_API AFunKTestBase : public AActor
{
	GENERATED_BODY()

public:
	AFunKTestBase();
	
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsStandaloneModeTest() const;
	
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsDedicatedServerModeTest() const;
	
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsListenServerModeTest() const;

	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnDedicatedServer() const;

	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnDedicatedServerClients() const;

	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnListenServer() const;

	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunOnListenServerClients() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	EFunKTestResult GetTestResult() const { return Result; }

	UFUNCTION(BlueprintCallable, Category="FunK")
	int32 GetSeed() const { return Seed; }

	UFUNCTION(BlueprintCallable, Category="FunK")
	FORCEINLINE FName GetStageName() const;
	
	const FFunKStage* GetCurrentStage() const;
	int32 GetCurrentStageIndex() const { return CurrentStageIndex; }
	const FFunKStages* GetStages() const;
	int32 GetCurrentStageExecutionTime() const { return CurrentStageExecutionTime; }

	virtual void BeginTest(int32 InTestRunID, int32 InSeed, int32 Variation);
	virtual void FinishTest(const FString& Reason = "");
	virtual void FinishTest(EFunKTestResult InResult, const FString& Reason = "");
	
	virtual void FinishStage();
	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishStage(EFunKStageResult StageResult, const FString& Message);

	UFunKWorldSubsystem* GetWorldSubsystem() const;
	UFunKEventBusSubsystem* GetEventBusSubsystem() const;

	FFunKTestFinishing OnTestFinish;
	FFunKPeerStageFinishing OnPeerStageFinishing;

	const FFunKAnonymousBitmask& GetStagePeerState() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	int32 GetPeerIndex() const;

	static void RegisterEvents(UFunKEventBusSubsystem* EventBusSubsystem);

	const FFunKTestVariations& GetTestVariations();
	const FFunKTestVariations& GetTestVariationsConst() const;
	int32 GetTestVariationCount();

	UFUNCTION(BlueprintCallable)
	FFunKTestVariation GetCurrentVariation();

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
	
	virtual void OnBegin(const FFunKTestBeginEvent& BeginEvent);
	virtual void OnBeginStage(const FFunKTestStageBeginEvent& BeginEvent);
	virtual void OnInvokeStage();
	virtual void OnFinishStage(EFunKStageResult StageResult, FString Message);
	virtual void OnFinish(const FString& Message);

	virtual void SetupStages(FFunKStagesSetup& stages);
	const FFunKStage* GetStage(int32 StageIndex) const;
	virtual bool IsExecutingStage(const FFunKStage& stage) const;
	bool IsStageTickDelegateBound(int32 StageIndex);
	bool IsValidStageIndex(int32 StageIndex) const;

	virtual void GatherContext(FFunKEvent& Event) const;

	bool IsServer() const;
	bool IsDriver() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Info(const FString& Message, const FString& Context = "") const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Warning(const FString& Message, const FString& Context = "") const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Error(const FString& Message, const FString& Context = "") const;
	
	UFUNCTION(BlueprintCallable, Category="FunK")
	UFunKTestFragment* AddTestFragment(UFunKTestFragment* Fragment);

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

	void BuildTestRegistry(FString& append) const;
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
	friend class UFunKTestFragmentBlueprintFunctionLibrary;
};

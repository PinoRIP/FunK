// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestEvents.h"
#include "FunKTestResult.h"
#include "EventBus/FunKEventBusRegistrationContainer.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Internal/Setup/FunKStages.h"
#include "Events/FunKEvent.h"
#include "Util/FunKAnonymousBitmask.h"
#include "FunKTestBase.generated.h"

struct FFunKStagesSetup;
class UFunKWorldSubsystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFunKTestFinishing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFunKPeerStageFinishing, int32, PeerIndex);

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

	UFUNCTION(BlueprintCallable)
	int32 GetPeerIndex() const;

	static void RegisterEvents(UFunKEventBusSubsystem* EventBusSubsystem);
protected:
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
	virtual void RaiseEvent(FFunKEvent& Event) const;

	bool IsServer() const;
	bool IsDriver() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Info(const FString& Message, const FString& Context = "") const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Warning(const FString& Message, const FString& Context = "") const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	void Error(const FString& Message, const FString& Context = "") const;

private:
	int32 Seed;
	int32 CurrentStageIndex = INDEX_NONE;
	int32 CurrentVariation = INDEX_NONE;
	int32 TestRunID;
	FFunKStages Stages;
	EFunKTestResult Result = EFunKTestResult::None;
	FFunKAnonymousBitmask PeerBitMask;

	bool IsLocalStageFinished = false;
	bool IsCurrentStageTickDelegateSetup = false;
	float CurrentStageExecutionTime = 0;

	FFunKEventBusRegistration BeginRegistration;
	FFunKEventBusRegistrationContainer RunningRegistrations;

	static bool IsDriver(ENetMode NetMode);
	
	UPROPERTY( replicated )
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

	void EndAllInputSimulations() const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR

	void BuildTestRegistry(FString& append) const;
	bool IsBpEventImplemented(const FName& Name) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	
	friend class UFunKAssertionBlueprintFunctionLibrary;
};

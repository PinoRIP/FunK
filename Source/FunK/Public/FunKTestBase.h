﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "EventBus/FunKEventBusRegistrationContainer.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Stages/FunKStages.h"
#include "Events/FunKEvent.h"
#include "Events/Internal/FunKTestFinishedEvent.h"
#include "Events/Internal/FunKTestStageFinishedEvent.h"
#include "Events/Internal/FunKTestStageEvent.h"
#include "Util/FunKAnonymousBitmask.h"

#include "FunKTestBase.generated.h"

class UFunKEventBusSubsystem;
class UFunKWorldSubsystem;
struct FFunKStagesSetup;
class AFunKWorldTestController;

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

	virtual void BeginTest(FGuid InTestRunID, int32 InSeed);

	FORCEINLINE FName GetStageName() const;
	
	virtual void FinishStage();
	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishStage(EFunKStageResult TestResult, const FString& Message);

	FORCEINLINE FFunKEvent CreateEvent(EFunKTestResult testResult, const FString& Message) const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsStageRunning() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	EFunKTestResult GetTestResult() const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	int32 GetSeed() const;

	bool IsBpEventImplemented(const FName& Name) const;

protected:
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FunK", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsEnabled = true;
	
	virtual void OnBegin();
	virtual void OnBeginStage();
	virtual void OnFinishStage(EFunKStageResult StageResult, FString Message);
	virtual void OnFinish(const FString& Message);

	virtual bool IsLastStage();
	virtual bool IsExecutingStage(const FFunKStage& stage) const;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupStages(FFunKStagesSetup& stages);

	const FFunKStage* GetCurrentStage() const;
	const FFunKStage* GetStage(int32 StageIndex) const;

	bool IsStageTickDelegateBound(int32 StageIndex);
	bool IsValidStageIndex(int32 StageIndex) const;
	int32 GetCurrentStageIndex() const { return CurrentStageIndex; }

	UFunKWorldSubsystem* GetWorldSubsystem() const;

	UFunKEventBusSubsystem* GetEventBusSubsystem() const;
	
private:
	int32 Seed;
	int32 CurrentStageIndex = INDEX_NONE;
	FGuid TestRunID;
	FFunKStages Stages;
	EFunKTestResult Result = EFunKTestResult::None;
	FFunKAnonymousBitmask PeerBitMask;

	FFunKEventBusRegistration BeginRegistration;
	FFunKEventBusRegistrationContainer RunningRegistrations;
	
	bool IsCurrentStageTickDelegateSetup = false;
	void OnBeginStage(const FFunKTestStageEvent& Event);
	void UpdateStageState(const FFunKTestStageEvent& Event);
	void OnBeginFirstStage(const FFunKTestStageEvent& Event);
	void OnFinishStage(const FFunKTestStageFinishedEvent& Event);
	void OnFinish(const FFunKTestFinishedEvent& Event);
	void NextStage(FGuid InTestRunID, int32 InSeed);
	
	void SetupStages();
	FFunKStage* GetCurrentStageMutable();
	FFunKStage* GetStageMutable(int32 StageIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const FFunKStages* GetStages() const;

	void BuildTestRegistry(FString& append) const;
	
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR

private:
};

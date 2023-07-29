// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "FunKTestRunID.h"
#include "Sinks/FunKSink.h"
#include "Stages/FunKStages.h"
#include "FunKTestBase.generated.h"

struct FFunKStagesSetup;
class AFunKWorldTestController;

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class FUNK_API AFunKTestBase : public AActor, public IFunKSink
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

	virtual void BeginTest(AFunKWorldTestController* Controller, FFunKTestRunID InTestRunID, int32 InSeed);
	virtual void BeginTestStage(int32 StageIndex);

	FORCEINLINE FName GetStageName() const;
	
	virtual void FinishStage();
	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishStage(EFunKTestResult TestResult, const FString& Message);

	FORCEINLINE FFunKEvent CreateEvent(EFunKTestResult testResult, const FString& Message) const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsStageRunning() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	EFunKTestResult GetTestResult() const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;
	
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;

	int32 GetCurrentSeed() const;

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
	virtual void OnBeginStage(const FName& StageName);
	virtual void OnFinishStage(const FName& StageName);
	virtual void OnFinish(const FString& Message);
	
	virtual void CheckLocalTestController();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupStages(FFunKStagesSetup& stages);

	const FFunKStage* GetCurrentStage() const;
	const FFunKStage* GetStage(int32 StageIndex) const;

	bool IsStageTickDelegateBound(int32 StageIndex);
	bool IsValidStageIndex(int32 StageIndex) const;
	int32 GetCurrentStageIndex() const { return CurrentStageIndex; }

	AFunKWorldTestController* GetCurrentController() const { return CurrentController; }
	
private:
	int32 Seed;
	int32 CurrentStageIndex = INDEX_NONE;
	FFunKTestRunID TestRunID;
	FFunKStages Stages;
	EFunKTestResult Result = EFunKTestResult::None;
	
	bool IsCurrentStageTickDelegateSetup = false;

	UPROPERTY()
	AFunKWorldTestController* CurrentController;
	
	void SetupStages();
	FFunKStage* GetCurrentStageMutable();
	FFunKStage* GetStageMutable(int32 StageIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const FFunKStages* GetStages() const;

	void BuildTestRegistry(FString& append) const;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR

private:
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKFunctionalTestResult.h"
#include "FunKWorldTestExecution.h"
#include "Stages/FunKStages.h"
#include "UObject/Object.h"
#include "FunKTestBase.generated.h"

struct FFunKStagesSetup;

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

	virtual void BeginTest(AFunKWorldTestController* Controller, FFunKTestID testId);
	virtual void BeginTestStage();

	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishTest(EFunKFunctionalTestResult InTestResult, const FString& Message);

	FORCEINLINE FFunKEvent CreateEvent(EFunKFunctionalTestResult testResult, const FString& Message) const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsStarted() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	EFunKFunctionalTestResult GetTestResult() const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	virtual FFunKTimeLimit* GetPreparationTimeLimit();
	virtual FFunKTimeLimit* GetTimeLimit();
	virtual FFunKTimeLimit* GetNetworkingTimeLimit();
	
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;

protected:
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FunK", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsEnabled = true;
	
	virtual bool InvokeAssume();
	virtual void InvokeStartSetup();
	virtual bool InvokeIsReady();
	virtual void InvokeStartTest();
	virtual void CleanupAfterTest();
	virtual void CheckLocalTestController();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;

	virtual void SetupStages(FFunKStagesSetup& stages);
private:
	EFunKFunctionalTestResult TestResult = EFunKFunctionalTestResult::None;
	
	FFunKTestID TestID;
	
	UPROPERTY()
	AFunKWorldTestController* CurrentController;

	FFunKStages Stages;
	void SetupStages();

	bool IsTestStarted = false;
	bool IsSetupReady = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const FFunKStages* GetStages() const;

	void BuildTestRegistry(FString& append) const;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR
};

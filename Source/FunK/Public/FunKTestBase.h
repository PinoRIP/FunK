// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKFunctionalTestResult.h"
#include "FunKWorldTestExecution.h"
#include "UObject/Object.h"
#include "FunKTestBase.generated.h"

USTRUCT(BlueprintType)
struct FFunKTimeLimit
{
	GENERATED_BODY()

	/** Test's time limit. '0' means no limit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Time = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFunKFunctionalTestResult Result = EFunKFunctionalTestResult::Failed;

	bool IsTimeout(float time) const;

	bool IsLimitless() const;
};

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
	bool IsRunningInStandaloneMode() const;
	
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunningInDedicatedServerMode() const;
	
	UFUNCTION(BlueprintCallable, Category="FunK|Setup")
	bool IsRunningInListenServerMode() const;

	virtual void BeginTestSetup(AFunKWorldTestController* Controller, FFunKTestID testId);
	virtual void BeginTestExecution();

	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishTest(EFunKFunctionalTestResult InTestResult, const FString& Message);

	FORCEINLINE FFunKEvent CreateEvent(EFunKFunctionalTestResult testResult, const FString& Message) const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsStarted() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	EFunKFunctionalTestResult GetTestResult() const;

	void BuildTestRegistry(FString& append) const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	FFunKTimeLimit* GetPreparationTimeLimit();
	FFunKTimeLimit* GetTimeLimit();
	FFunKTimeLimit* GetNetworkingTimeLimit();

protected:
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FunK", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnStandalone = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnDedicatedServer = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnDedicatedServerClients = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnListenServer = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnListenServerClients = true;
	
	virtual bool InvokeAssume();
	virtual void InvokeStartSetup();
	virtual bool InvokeIsReady();
	virtual void InvokeStartTest();
	virtual void InvokeCleanup();
	virtual void InvokeCheckForLocalTestController();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;
private:
	EFunKFunctionalTestResult TestResult = EFunKFunctionalTestResult::None;
	
	FFunKTestID TestID;
	
	UPROPERTY()
	AFunKWorldTestController* CurrentController;

	bool IsTestStarted = false;
	bool IsSetupReady = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR
};

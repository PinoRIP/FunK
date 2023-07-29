// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKFunctionalTestResult.h"
#include "FunKWorldTestExecution.h"
#include "GameFramework/Actor.h"
#include "Sinks/FunKSink.h"
#include "FunkFunctionalTest.generated.h"

class AFunKWorldTestController;
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


UCLASS()
class FUNK_API AFunKFunctionalTest : public AActor, public IFunKSink
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunKFunctionalTest();
	
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FunK", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsEnabled = true;

	/** The Test's time limit for preparation, this is the time it has to return true when checking IsReady(). '0' means no limit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit PreparationTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit TimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit NetworkingTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunInStandaloneMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunInDedicatedServerMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunInListenServerMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnServer = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnClient = true;

	uint32 RunFrame;
	float RunTime;
	uint32 StartFrame;
	float StartTime;
	bool IsTestStarted = false;

	float TotalTime;
	float PreparationTime;
	float ExecutionTime;

	bool IsSetupReady;

	virtual void BeginTestSetup(AFunKWorldTestController* Controller, FFunKTestId testId);
	virtual void BeginTestExecution();

	UFUNCTION(BlueprintCallable, Category="FunK")
	virtual void FinishTest(EFunKFunctionalTestResult TestResult, const FString& Message);

	FORCEINLINE static FFunKEvent CreateEvent(EFunKFunctionalTestResult testResult, const FString& Message);

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsStarted() const;

	UFUNCTION(BlueprintCallable, Category="FunK")
	bool IsFinished() const;

	EFunKFunctionalTestResult GetTestResult() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	bool BpAssume();

	virtual void StartSetup();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="StartSetup")
	void BpStartSetup();
	
	virtual bool IsReady();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="IsReady")
	bool BpReady();

	virtual void StartTest();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="StartTest")
	void BpStartTest();

	virtual void Cleanup();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Cleanup")
	void BpCleanup();

	EFunKFunctionalTestResult TestResult = EFunKFunctionalTestResult::Default;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	FFunKTestId TestId;
	
	UPROPERTY()
	AFunKWorldTestController* Controller;

	friend class AFunKWorldTestController;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BuildTestRegistry(FString& append) const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;
	
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR
};

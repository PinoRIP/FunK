// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestBase.h"
#include "Setup/FunKTimeLimit.h"
#include "FunkFunctionalTest.generated.h"

class AFunKWorldTestController;

struct FFunKVariationCount
{
	int32 Root = 0;
};

/*
 * Functional test class
 */
UCLASS(Blueprintable)
class FUNK_API AFunKFunctionalTest : public AFunKTestBase
{
	GENERATED_BODY()

public:
	AFunKFunctionalTest();

protected:
	// Limit for the arrange stage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeTimeLimit;

	// Limit for the act stage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ActTimeLimit;

	// Should the test run on standalone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnStandalone = true;

	// Should the test run on dedicated server
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnDedicatedServer = true;

	// Should the test run on dedicated server clients
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnDedicatedServerClients = true;

	// Should the test run on listen server
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnListenServer = true;
	
	// Should the test run on listen server clients
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup")
	bool RunOnListenServerClients = true;

	// Make the assert stage latent
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FunK|Setup|Assert")
	bool LatentAssert = false;

	// Limit for the assert stage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Assert", meta=(EditCondition="LatentAssert"))
	FFunKTimeLimit AssertTimeLimit;

	virtual void SetupStages(FFunKStagesSetup& stages) override final;
	virtual void SetupFunctionalTestStages(FFunKStagesSetup& stages);

	void InvokeAssume();
	void InvokeArrange();
	void InvokeAssert();

	// Assume stage
	virtual bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	// Assume stage
	bool BpAssume();

	// Arrange stage
	virtual bool Arrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Arrange")
	// Arrange stage
	void BpArrange();
	// Tick while the arrange stage is active
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ArrangeTick")
	void BpArrangeTick(float DeltaTime);

	// Act stage
	virtual void Act();
	// Act stage
	UFUNCTION(BlueprintImplementableEvent, Category="FunK", DisplayName="Act")
	void BpAct();
	// Tick while the act stage is active
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ActTick")
	void BpActTick(float DeltaTime);

	// Assert stage
	virtual bool Assert();
	// Assert stage
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assert")
	void BpAssert();
	// Tick while the assert stage is active (only valid for latent assert)
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ActTick")
	void BpAssertTick(float DeltaTime);

	virtual void OnFinish(const FString& Message) override;

	// Called on finish
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Cleanup")
	void BpCleanup();
};

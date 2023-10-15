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

UCLASS(Blueprintable)
class FUNK_API AFunKFunctionalTest : public AFunKTestBase
{
	GENERATED_BODY()

public:
	AFunKFunctionalTest();

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ActTimeLimit;
		
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FunK|Setup|Assert")
	bool LatentAssert = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Assert", meta=(EditCondition="LatentAssert"))
	FFunKTimeLimit AssertTimeLimit;

	virtual void SetupStages(FFunKStagesSetup& stages) override final;
	virtual void SetupFunctionalTestStages(FFunKStagesSetup& stages);

	void InvokeAssume();
	void InvokeArrange();
	void InvokeAssert();

	virtual bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	bool BpAssume();

	virtual bool Arrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Arrange")
	void BpArrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ArrangeTick")
	void BpArrangeTick(float DeltaTime);
	
	virtual void Act();
	UFUNCTION(BlueprintImplementableEvent, Category="FunK", DisplayName="Act")
	void BpAct();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ActTick")
	void BpActTick(float DeltaTime);

	virtual bool Assert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assert")
	void BpAssert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ActTick")
	void BpAssertTick(float DeltaTime);

	virtual void OnFinish(const FString& Message) override;

	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Cleanup")
	void BpCleanup();
};

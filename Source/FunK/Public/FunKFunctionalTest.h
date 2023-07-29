// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestBase.h"
#include "FunkFunctionalTest.generated.h"

class AFunKWorldTestController;



UCLASS()
class FUNK_API AFunKFunctionalTest : public AFunKTestBase
{
	GENERATED_BODY()

public:
	AFunKFunctionalTest();
	
	virtual FFunKTimeLimit* GetSyncTimeLimit() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ActTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit SyncTimeLimit;
	
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

	virtual void SetupStages(FFunKStagesSetup& stages) override final;
	virtual void SetupFunctionalTestStages(FFunKStagesSetup& stages);

	void InvokeAssume();
	void InvokeArrange();

	virtual bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	bool BpAssume();

	virtual bool Arrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Arrange")
	bool BpArrange();
	
	virtual void Act();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Act")
	void BpAct();

	virtual void Assert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assert")
	void BpAssert();
};

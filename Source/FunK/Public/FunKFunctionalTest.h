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
	
	virtual FFunKTimeLimit* GetPreparationTimeLimit() override;
	virtual FFunKTimeLimit* GetTimeLimit() override;
	virtual FFunKTimeLimit* GetNetworkingTimeLimit() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit PreparationTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit TimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit NetworkingTimeLimit;
	
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

	virtual void SetupStages(FFunKStagesSetup& stages) override;
	
	virtual bool InvokeAssume() override;
	virtual void InvokeStartSetup() override;
	virtual bool InvokeIsReady() override;
	virtual void InvokeStartTest() override;
	virtual void CleanupAfterTest() override;

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
};

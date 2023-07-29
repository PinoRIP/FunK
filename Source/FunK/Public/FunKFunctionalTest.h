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


protected:
	virtual bool InvokeAssume() override;
	virtual void InvokeStartSetup() override;
	virtual bool InvokeIsReady() override;
	virtual void InvokeStartTest() override;
	virtual void InvokeCleanup() override;

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKTestFunctionality.generated.h"

class AFunKTestBase;
/**
 * 
 */
UCLASS(Abstract, HideDropdown)
class FUNK_API UFunKTestFunctionality : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnAdded();
	virtual void OnBeginStage();
	virtual void OnFinishStage();
	virtual void OnRemoved();

	virtual FString GetReadableIdent() const;

protected:
	UFUNCTION(BlueprintCallable)
	AFunKTestBase* GetTest();

	UFUNCTION(BlueprintCallable)
	void Error(const FString& Message) const;
	
private:
	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	friend AFunKTestBase;
};

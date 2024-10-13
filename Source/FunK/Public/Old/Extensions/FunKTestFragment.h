// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKTestFragment.generated.h"

class AFunKTestBase;

/**
 * Test fragments can be implemented to share functionality between tests. They can be added manually or by variation component. 
 */
UCLASS(Abstract, HideDropdown)
class FUNK_API UFunKTestFragment : public UObject
{
	GENERATED_BODY()

public:
	// Called when the fragment is added to a test
	virtual void OnAdded();

	// Called when a stage starts (when the fragment is added as "StageFragment" this will be called immediately)
	virtual void OnBeginStage();

	// Called when a stage ends
	virtual void OnFinishStage();

	// Called when the fragment is removed from the test
	virtual void OnRemoved();

	// Called to get a readable name for the fragment instance
	virtual FString GetLogName() const;

protected:
	// Gets the test the fragment is added to.
	UFUNCTION(BlueprintCallable)
	AFunKTestBase* GetTest();

	// Sends an error (no matter the net location) & fails the test.
	UFUNCTION(BlueprintCallable)
	void Error(const FString& Message) const;
	
private:
	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	friend AFunKTestBase;
};

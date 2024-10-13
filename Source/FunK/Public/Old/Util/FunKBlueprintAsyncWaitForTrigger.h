// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FunKBlueprintAsyncWaitForTrigger.generated.h"

class ATriggerBase;
class AFunKTestBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFunKTriggerTriggered, AActor*, Actor);

/**
 * 
 */
UCLASS()
class FUNK_API UFunKBlueprintAsyncWaitForTrigger : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintAssignable)
	FFunKTriggerTriggered OnTriggered;

	// Wait for an actor to trigger the given trigger
	UFUNCTION(BlueprintCallable, meta = (HidePin = "Test", DefaultToSelf = "Test", BlueprintInternalUseOnly = "true"), Category = "AsyncNode", DisplayName="WaitFor (Trigger)")
	static UFunKBlueprintAsyncWaitForTrigger* WaitFor(AFunKTestBase* Test, ATriggerBase* Trigger);
	
	virtual void Activate() override;

	virtual void SetReadyToDestroy() override;

private:
	UPROPERTY()
	ATriggerBase* Trigger = nullptr;
	
	UFUNCTION()
	void OnTriggeredActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};

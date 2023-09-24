// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKActorScenarioLocalOwnershipCheckFunctions.generated.h"

class AFunKActorScenarioTest;
/**
 * 
 */
UCLASS()
class FUNK_API UFunKActorScenarioLocalOwnershipCheckFunctions : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool IsOwnershipNone(AFunKActorScenarioTest* Test);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool IsOwnershipAppositionPlayer(AFunKActorScenarioTest* Test);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool IsOwnershipOppositionPlayer(AFunKActorScenarioTest* Test);
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool IsOwnershipAI(AFunKActorScenarioTest* Test);
};

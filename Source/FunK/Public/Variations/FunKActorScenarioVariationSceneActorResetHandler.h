// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKActorScenarioVariationSceneActorResetHandler.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FUNK_API UFunKActorScenarioVariationSceneActorResetHandler : public UObject
{
	GENERATED_BODY()

public:
	virtual void Reset(AActor* Actor) const;
};

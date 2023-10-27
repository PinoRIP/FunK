// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKSceneActorResetHandler.generated.h"

/**
 * Handler to reset a level actor.
 */
UCLASS(Abstract)
class FUNK_API UFunKSceneActorResetHandler : public UObject
{
	GENERATED_BODY()

public:
	// Gets called to initially capture the actor state
	virtual void Capture(AActor* Actor);

	// Get called to reset the actor to the initial state
	virtual void Reset(AActor* Actor);
};

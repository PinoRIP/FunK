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
	virtual void Capture(AActor* Actor);
	virtual void Reset(AActor* Actor);
};

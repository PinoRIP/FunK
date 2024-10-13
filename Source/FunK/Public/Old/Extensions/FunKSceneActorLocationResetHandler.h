// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Extensions/FunKSceneActorResetHandler.h"
#include "FunKSceneActorLocationResetHandler.generated.h"

/**
 * Handler to reset a level actors position
 */
UCLASS()
class FUNK_API UFunKSceneActorLocationResetHandler : public UFunKSceneActorResetHandler
{
	GENERATED_BODY()

public:
	virtual void Capture(AActor* Actor) override;
	virtual void Reset(AActor* Actor) override;

private:
	FVector Location;
};

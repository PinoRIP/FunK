// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKTestVariationsWorldActor.generated.h"

class UFunKTestVariationComponent;

/*
 * This actor can be spawned in a test world. TestVariationComponents added to this actor will then be used as variations for all tests in that world.
 * RootVariations can't be used that way.
 */
UCLASS()
class FUNK_API AFunKTestVariationsWorldActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunKTestVariationsWorldActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

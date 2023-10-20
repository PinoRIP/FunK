// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestFragment.h"
#include "Util/FunKNetworkEmulationTypes.h"
#include "Extensions/FunKTestVariationComponent.h"
#include "FunKNetworkEmulationVariationComponent.generated.h"


class UFunKNetworkEmulationVariationComponent;


/*
 * Variations for multiple network qualities.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKNetworkEmulationVariationComponent : public UFunKTestVariationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKNetworkEmulationVariationComponent();

	UPROPERTY(EditAnywhere, Category="FunK|Setup")
	TArray<FFunKNetworkEmulation> Emulations;

	virtual int32 GetCount() override;
	virtual UFunKTestFragment* GetFragment(int32 Index) override;
};

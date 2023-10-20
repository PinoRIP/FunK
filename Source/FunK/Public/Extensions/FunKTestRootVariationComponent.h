// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Extensions/FunKTestVariationComponent.h"
#include "FunKTestRootVariationComponent.generated.h"


UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKTestRootVariationComponent : public UFunKTestVariationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKTestRootVariationComponent();
};

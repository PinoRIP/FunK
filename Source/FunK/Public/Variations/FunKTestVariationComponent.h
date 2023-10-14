// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FunKTestVariationComponent.generated.h"


class UFunKTestFunctionality;
class AFunKTestBase;

UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKTestVariationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKTestVariationComponent();

	virtual int32 GetCount();
	virtual UFunKTestFunctionality* GetFunctionality(int32 Index);
	virtual bool IsReady(UFunKTestFunctionality* Instance, int32 Index);
	virtual void OnUsing(UFunKTestFunctionality* Instance);
};

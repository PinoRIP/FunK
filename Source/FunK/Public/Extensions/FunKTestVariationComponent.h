// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FunKTestVariationComponent.generated.h"


class UFunKTestFragment;
class AFunKTestBase;

UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKTestVariationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKTestVariationComponent();

	virtual int32 GetCount();
	virtual UFunKTestFragment* GetFragment(int32 Index);
	virtual bool IsReady(UFunKTestFragment* Instance, int32 Index);
	virtual void OnUsing(UFunKTestFragment* Instance);
};

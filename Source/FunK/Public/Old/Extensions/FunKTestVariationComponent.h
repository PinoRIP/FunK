// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Extensions/FunKTestFragment.h"
#include "Components/ActorComponent.h"
#include "FunKTestVariationComponent.generated.h"


class UTestFunKTestVariationComponent;
class AFunKTestBase;

/*
 * Dummy test variation fragment
 */
UCLASS(NotBlueprintType, HideDropdown)
class UFunKTestVariationNoFragment : public UFunKTestFragment
{
	GENERATED_BODY()

public:
	virtual FString GetLogName() const override;
};

/*
 * With variation components a test can be run multiple times but with slight variations. One component can be responsible for multiple repetitions. The variation can be provided by the returned test fragment, or manually implemented in the test.
 * TestVariationComponents can be shared in a world via. "AFunKTestVariationsWorldActor".
 */
UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKTestVariationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKTestVariationComponent();

	// Returns the amount of repetitions this component is responsible for (This has to be the same value on server & client, but can be different between dedicated/listen/standalone) 
	virtual int32 GetCount();

	// Provides a fragment that will be automatically added to the test. These fragments can also be networked.
	virtual UFunKTestFragment* GetFragment(int32 Index);

	// Return true when everything is setup
	virtual bool IsReady(UFunKTestFragment* Instance, int32 Index);

	// Callback when a fragment is in use. (When the fragment is networked, on clients this will be the replicated version instead the one provided by "GetFragment")
	virtual void OnUsing(UFunKTestFragment* Instance);

	virtual FString GetLogName() const;
};

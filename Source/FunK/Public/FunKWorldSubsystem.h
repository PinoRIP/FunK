// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKWorldSubsystem.generated.h"

class UFunKTestVariationComponent;
class AFunKTestBase;
class AFunKWorldTestController;

USTRUCT()
struct FFunKWorldVariations
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UFunKTestVariationComponent*> Variations;

	UPROPERTY()
	int32 VariationCount = 0;
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Gets the local test controller (spawns one if none is existing)
	AFunKWorldTestController* GetLocalTestController();

	// Gets the local peer index
	int32 GetPeerIndex() const;

	// Gets the amount of peers (Connected UE instances)
	int32 GetPeerCount() const;

	// true when the server is dedicated (works on clients)
	bool IsServerDedicated() const;

	// Gets the informations about all shared variations.
	const FFunKWorldVariations& GetWorldVariations();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
	bool AreVariationsGathered = false;
	
	UPROPERTY()
	AFunKWorldTestController* LocalTestController = nullptr;
	AFunKWorldTestController* NewTestController() const;

	UPROPERTY()
	FFunKWorldVariations Variations;
	void GatherVariations(FFunKWorldVariations& OutVariations) const;

	friend class AFunKTestBase;
};

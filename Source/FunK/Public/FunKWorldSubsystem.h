// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKWorldSubsystem.generated.h"

class UFunKTestVariationComponent;
class AFunKTestBase;
class AFunKWorldTestController;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	AFunKWorldTestController* GetLocalTestController();

	int32 GetPeerIndex() const;
	int32 GetPeerCount() const;
	bool IsServerDedicated() const;

	const TArray<UFunKTestVariationComponent*>& GetWorldVariations();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
private:
	bool AreVariationsGathered = false;
	
	UPROPERTY()
	AFunKWorldTestController* LocalTestController = nullptr;
	AFunKWorldTestController* NewTestController() const;

	UPROPERTY()
	TArray<UFunKTestVariationComponent*> Variations;
	void GatherVariations(TArray<UFunKTestVariationComponent*>& OutVariations) const;

	friend class AFunKTestBase;
};

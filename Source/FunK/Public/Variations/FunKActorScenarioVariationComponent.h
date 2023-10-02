// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRootVariationComponent.h"
#include "Components/ActorComponent.h"
#include "FunKActorScenarioVariationComponent.generated.h"

UENUM(BlueprintType)
enum class EFunKActorScenarioVariationOwnership : uint8
{
	None,
	AppositionPlayer,
	OppositionPlayer,

	//TODO: Setup AI
	AI,
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationSpawnActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EFunKActorScenarioVariationOwnership Ownership = EFunKActorScenarioVariationOwnership::None;
	
	UPROPERTY(EditAnywhere)
	AActor* SceneActor = nullptr;

	UPROPERTY(EditAnywhere)
	FFunKActorScenarioVariationSpawnActor SpawnActor;

	//TODO: Spawn handler?
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKActorScenarioVariationComponent : public UFunKTestRootVariationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKActorScenarioVariationComponent();

private:
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/FunK.EFunKTestLocationTarget"), Category="FunK|Setup")
	int32 Exclude = 0;

	UPROPERTY(EditAnywhere, Category="FunK|Setup")
	TArray<FFunKActorScenarioVariationActor> Actors;

public:
	virtual void Begin(int32 index) override;

protected:
	virtual AActor* AcquireActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* GetSceneActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* SpawnActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual void AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership);
	virtual void AssignAppositionPlayer(AActor* Actor);
	virtual void AssignOppositionPlayer(AActor* Actor);
	virtual void AssignAI(AActor* Actor);
};

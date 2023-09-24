// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Internal/ActorScenario/FunKActorScenario.h"
#include "Components/ActorComponent.h"
#include "FunKActorScenarioComponent.generated.h"

UENUM(BlueprintType)
enum class EFunKActorScenarioOwnership : uint8
{
	None,
	AppositionPlayer,
	OppositionPlayer,

	//TODO: Setup AI
	AI,
};

UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKActorScenarioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKActorScenarioComponent();

	virtual AActor* AcquireActor(const FFunKActorScenario& Scenario);
	virtual bool VerifyActor(AActor* Actor, const FFunKActorScenario& Scenario);
	virtual void ReleaseActor(AActor* Actor, const FFunKActorScenario& Scenario);

public:
	UPROPERTY(EditAnywhere, Category = "Ownership")
	EFunKActorScenarioOwnership ActorOwnership = EFunKActorScenarioOwnership::AppositionPlayer;
};

﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKActorScenarioSpawnActorComponent.h"
#include "Components/ActorComponent.h"
#include "FunKActorScenarioSpawnOwnedActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKActorScenarioSpawnOwnedActorComponent : public UFunKActorScenarioSpawnActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKActorScenarioSpawnOwnedActorComponent();

	virtual AActor* AcquireActor(const FFunKActorScenario& Scenario) override;
	virtual void AssignOwner(AActor* Actor, const FFunKActorScenario& Scenario);
	virtual void AssignOwner(AActor* Actor, APlayerController* PlayerController);
	virtual bool VerifyActor(AActor* Actor, const FFunKActorScenario& Scenario) override;
	virtual void ReleaseActor(AActor* Actor, const FFunKActorScenario& Scenario) override;

protected:
	APlayerController* GetController(const FFunKActorScenario& Scenario) const;
};

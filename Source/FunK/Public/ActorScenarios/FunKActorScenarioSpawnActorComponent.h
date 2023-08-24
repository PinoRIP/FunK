// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKActorScenarioComponent.h"
#include "FunKActorScenarioSpawnActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKActorScenarioSpawnActorComponent : public UFunKActorScenarioComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKActorScenarioSpawnActorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	TSubclassOf<AActor> TypeOfActorToSpawn;

	virtual AActor* AcquireActor(FFunKActorScenario& Scenario) override;
	virtual bool VerifyActor(AActor* Actor, FFunKActorScenario& Scenario) override;
	virtual void ReleaseActor(AActor* Actor, FFunKActorScenario& Scenario) override;
};

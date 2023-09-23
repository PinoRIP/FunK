// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Internal/ActorScenario/FunKActorScenario.h"
#include "Components/ActorComponent.h"
#include "FunKActorScenarioComponent.generated.h"


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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsOppositionActor = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsStandaloneRelevant = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsDedicatedServerRelevant = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsDedicatedServerClientRelevant = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsListenServerRelevant = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK")
	bool IsListenServerClientRelevant = true;
};

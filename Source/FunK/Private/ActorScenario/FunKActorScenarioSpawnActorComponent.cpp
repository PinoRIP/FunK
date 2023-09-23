// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenario/FunKActorScenarioSpawnActorComponent.h"


// Sets default values for this component's properties
UFunKActorScenarioSpawnActorComponent::UFunKActorScenarioSpawnActorComponent()
{
}

AActor* UFunKActorScenarioSpawnActorComponent::AcquireActor(const FFunKActorScenario& Scenario)
{
	AActor* Actor = GetWorld()->SpawnActor(TypeOfActorToSpawn);
	Actor->bAlwaysRelevant = true;
	return Actor;
}

bool UFunKActorScenarioSpawnActorComponent::VerifyActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
	return Super::VerifyActor(Actor, Scenario) && Actor->HasActorBegunPlay();
}

void UFunKActorScenarioSpawnActorComponent::ReleaseActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
	GetWorld()->DestroyActor(Actor);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenarios/FunKActorScenarioSpawnActorComponent.h"


// Sets default values for this component's properties
UFunKActorScenarioSpawnActorComponent::UFunKActorScenarioSpawnActorComponent()
{
}

AActor* UFunKActorScenarioSpawnActorComponent::AcquireActor()
{
	return GetWorld()->SpawnActor(TypeOfActorToSpawn);
}

void UFunKActorScenarioSpawnActorComponent::ReleaseActor(AActor* Actor)
{
	GetWorld()->DestroyActor(Actor);
}


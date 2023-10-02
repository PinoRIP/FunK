// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations/FunKActorScenarioVariationComponent.h"


// Sets default values for this component's properties
UFunKActorScenarioVariationComponent::UFunKActorScenarioVariationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UFunKActorScenarioVariationComponent::Begin(int32 index)
{
	Super::Begin(index);
}

AActor* UFunKActorScenarioVariationComponent::AcquireActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	if(VariationActor.SceneActor)
	{
		return GetSceneActor(VariationActor);
	}
	else
	{
		return SpawnActor(VariationActor);
	}
}

AActor* UFunKActorScenarioVariationComponent::GetSceneActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	return VariationActor.SceneActor;
}

AActor* UFunKActorScenarioVariationComponent::SpawnActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	return GetWorld()->SpawnActor(VariationActor.SpawnActor.Class);
}

void UFunKActorScenarioVariationComponent::AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership)
{
	switch (Ownership)
	{
	case EFunKActorScenarioVariationOwnership::AppositionPlayer:
		AssignAppositionPlayer(Actor);
		break;
	case EFunKActorScenarioVariationOwnership::OppositionPlayer:
		AssignOppositionPlayer(Actor);
		break;
	case EFunKActorScenarioVariationOwnership::AI:
		AssignAI(Actor);
		break;
	default: ;
	}
}

void UFunKActorScenarioVariationComponent::AssignAppositionPlayer(AActor* Actor)
{
}

void UFunKActorScenarioVariationComponent::AssignOppositionPlayer(AActor* Actor)
{
}

void UFunKActorScenarioVariationComponent::AssignAI(AActor* Actor)
{
}

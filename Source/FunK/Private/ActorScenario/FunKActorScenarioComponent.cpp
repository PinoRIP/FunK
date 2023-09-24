// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenario/FunKActorScenarioComponent.h"


// Sets default values for this component's properties
UFunKActorScenarioComponent::UFunKActorScenarioComponent()
{
}

AActor* UFunKActorScenarioComponent::AcquireActor(const FFunKActorScenario& Scenario)
{
	return nullptr;
}

bool UFunKActorScenarioComponent::VerifyActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
	return Actor != nullptr;
}

void UFunKActorScenarioComponent::ReleaseActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
}


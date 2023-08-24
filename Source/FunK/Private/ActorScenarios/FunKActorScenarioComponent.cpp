// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenarios/FunKActorScenarioComponent.h"


// Sets default values for this component's properties
UFunKActorScenarioComponent::UFunKActorScenarioComponent()
{
}

AActor* UFunKActorScenarioComponent::AcquireActor(FFunKActorScenario& Scenario)
{
	return nullptr;
}

bool UFunKActorScenarioComponent::VerifyActor(AActor* Actor, FFunKActorScenario& Scenario)
{
	return Actor != nullptr;
}

void UFunKActorScenarioComponent::ReleaseActor(AActor* Actor, FFunKActorScenario& Scenario)
{
}

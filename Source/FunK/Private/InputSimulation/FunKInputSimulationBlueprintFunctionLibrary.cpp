// Fill out your copyright notice in the Description page of Project Settings.


#include "InputSimulation/FunKInputSimulationBlueprintFunctionLibrary.h"
#include "InputSimulation/FunKInputSimulationSystem.h"

void UFunKInputSimulationBlueprintFunctionLibrary::EndInputSimulation(UObject* WorldContext, const FFunKInputSimulationHandle& Handle)
{
	UFunKInputSimulationSystem* InputSimulationSystem = WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
	InputSimulationSystem->EndInputActionSimulation(Handle.Key);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndInputSimulations(UObject* WorldContext, const TArray<FFunKInputSimulationHandle>& Handles)
{
	UFunKInputSimulationSystem* InputSimulationSystem = WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
	for (const FFunKInputSimulationHandle& Handle : Handles)
	{
		InputSimulationSystem->EndInputActionSimulation(Handle.Key);
	}
}

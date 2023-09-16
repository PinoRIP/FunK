// Fill out your copyright notice in the Description page of Project Settings.


#include "InputSimulation/FunKInputSimulationBlueprintFunctionLibrary.h"

#include "GameFramework/PlayerController.h"
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

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputActionOnce(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputActionOnce(InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputActionOnce(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateInputActionOnce(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputActionFor(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputActionFor(InputAction, InputActionValue, applicationTimeMs);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputActionFor(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateInputActionFor(PlayerController, InputAction, InputActionValue, applicationTimeMs);
}

FFunKInputSimulationHandle UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	return GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputAction(InputAction, InputActionValue);
}

FFunKInputSimulationHandle UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	return GetInputSimulationSystem(PlayerController)
			->SimulateInputAction(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyFirstPlayerActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyFirstPlayerActionInput(ActionName, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyActionInput(PlayerController, ActionName, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyFirstPlayerAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyFirstPlayerAxisInput(AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyAxisInput(PlayerController, AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerKeyPressInput(PressedKey, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateKeyPressInput(PlayerController, PressedKey, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue)
{
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerKeyAxisInput(AxisKey, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue)
{
	GetInputSimulationSystem(PlayerController)
		->SimulateKeyAxisInput(PlayerController, AxisKey, AxisValue);
}

UFunKInputSimulationSystem* UFunKInputSimulationBlueprintFunctionLibrary::GetInputSimulationSystem(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
}

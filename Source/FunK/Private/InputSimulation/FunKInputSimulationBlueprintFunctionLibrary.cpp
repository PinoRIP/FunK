// Fill out your copyright notice in the Description page of Project Settings.


#include "InputSimulation/FunKInputSimulationBlueprintFunctionLibrary.h"

#include "GameFramework/PlayerController.h"
#include "InputSimulation/FunKInputSimulationSystem.h"

void UFunKInputSimulationBlueprintFunctionLibrary::EndInputSimulation(UObject* WorldContext, const FFunKInputSimulationHandle& Handle)
{
	if(!WorldContext) return;
	UFunKInputSimulationSystem* InputSimulationSystem = WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
	InputSimulationSystem->EndInputActionSimulation(Handle.Key);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndInputSimulations(UObject* WorldContext, const TArray<FFunKInputSimulationHandle>& Handles)
{
	if(!WorldContext) return;
	UFunKInputSimulationSystem* InputSimulationSystem = WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
	for (const FFunKInputSimulationHandle& Handle : Handles)
	{
		InputSimulationSystem->EndInputActionSimulation(Handle.Key);
	}
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputActionOnce(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputActionOnce(InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputActionOnce(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateInputActionOnce(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputActionFor(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputActionFor(InputAction, InputActionValue, applicationTimeMs);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputActionFor(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateInputActionFor(PlayerController, InputAction, InputActionValue, applicationTimeMs);
}

FFunKInputSimulationHandle UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if(!WorldContext) FFunKInputSimulationHandle(-1);
	return GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerInputAction(InputAction, InputActionValue);
}

FFunKInputSimulationHandle UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if(!PlayerController) FFunKInputSimulationHandle(-1);
	return GetInputSimulationSystem(PlayerController)
			->SimulateInputAction(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyFirstPlayerActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyFirstPlayerActionInput(ActionName, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyActionInput(PlayerController, ActionName, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyFirstPlayerAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyFirstPlayerAxisInput(AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyAxisInput(PlayerController, AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerKeyPressInput(PressedKey, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateKeyPressInput(PlayerController, PressedKey, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateFirstPlayerKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue)
{
	if(!WorldContext) return;
	GetInputSimulationSystem(WorldContext)
		->SimulateFirstPlayerKeyAxisInput(AxisKey, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue)
{
	if(!PlayerController) return;
	GetInputSimulationSystem(PlayerController)
		->SimulateKeyAxisInput(PlayerController, AxisKey, AxisValue);
}

UFunKInputSimulationSystem* UFunKInputSimulationBlueprintFunctionLibrary::GetInputSimulationSystem(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
}

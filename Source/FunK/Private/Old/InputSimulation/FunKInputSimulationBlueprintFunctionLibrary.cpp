// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/InputSimulation/FunKInputSimulationBlueprintFunctionLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Old/InputSimulation/FunKInputSimulationSystem.h"

void UFunKInputSimulationBlueprintFunctionLibrary::EndAllInputSimulations(UObject* WorldContext)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->EndAllInputSimulations();
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->SimulateInputAction(InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
			->SimulateControllerInputAction(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateInputAction(UObject* WorldContext, const UInputAction* InputAction)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->EndSimulateInputAction(InputAction);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->EndSimulateControllerInputAction(PlayerController, InputAction);
}


void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyActionInput(ActionName, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyControllerActionInput(PlayerController, ActionName, InputEventType);
}


void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->SimulateLegacyAxisInput(AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->SimulateLegacyControllerAxisInput(PlayerController, AxisName, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->EndSimulateLegacyAxisInput(AxisName);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->EndSimulateLegacyControllerAxisInput(PlayerController, AxisName);
}


void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->SimulateKeyPressInput(PressedKey, InputEventType);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->SimulateControllerKeyPressInput(PlayerController, PressedKey, InputEventType);
}


void UFunKInputSimulationBlueprintFunctionLibrary::SimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->SimulateKeyAxisInput(AxisKey, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->SimulateControllerKeyAxisInput(PlayerController, AxisKey, AxisValue);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey)
{
	if (!WorldContext)
		return;
	
	GetInputSimulationSystem(WorldContext)
		->EndSimulateKeyAxisInput(AxisKey);
}

void UFunKInputSimulationBlueprintFunctionLibrary::EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey)
{
	if (!PlayerController)
		return;
	
	GetInputSimulationSystem(PlayerController)
		->EndSimulateControllerKeyAxisInput(PlayerController, AxisKey);
}


UFunKInputSimulationSystem* UFunKInputSimulationBlueprintFunctionLibrary::GetInputSimulationSystem(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetSubsystem<UFunKInputSimulationSystem>();
}

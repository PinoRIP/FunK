// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKInputSimulationBlueprintFunctionLibrary.generated.h"

struct FInputActionValue;
class UInputAction;
struct FFunKInputSimulationHandle;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKInputSimulationBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulationHandle", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndInputSimulation(UObject* WorldContext, const FFunKInputSimulationHandle& Handle);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulationHandle", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndInputSimulations(UObject* WorldContext, const TArray<FFunKInputSimulationHandle>& Handles);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateFirstPlayerInputActionOnce(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateInputActionOnce(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateFirstPlayerInputActionFor(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateInputActionFor(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static FFunKInputSimulationHandle SimulateFirstPlayerInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static FFunKInputSimulationHandle SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyFirstPlayerActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyFirstPlayerAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateFirstPlayerKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateFirstPlayerKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);
	
private:
	static class UFunKInputSimulationSystem* GetInputSimulationSystem(const UObject* WorldContext);
};

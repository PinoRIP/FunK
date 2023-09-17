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
	static void EndAllInputSimulations(UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	void EndSimulateInputAction(UObject* WorldContext, const UInputAction* InputAction);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	void EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction);

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);
		
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndSimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName);
	
public:
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);
		
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndSimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey);

private:
	static class UFunKInputSimulationSystem* GetInputSimulationSystem(const UObject* WorldContext);
};

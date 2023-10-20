// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKInputSimulationBlueprintFunctionLibrary.generated.h"

struct FInputActionValue;
class UInputAction;
struct FFunKInputSimulationHandle;

/**
 * Wrapper around the UFunKInputSimulationSystem for easy access in BP
 */
UCLASS()
class FUNK_API UFunKInputSimulationBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Ends any kind of input simulation in this world (on the peer only)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulationHandle", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndAllInputSimulations(UObject* WorldContext);

	// Simulates the given input action value for the given input action until the simulation is ended with "EndSimulateInputAction"
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateInputAction(UObject* WorldContext, const UInputAction* InputAction, FInputActionValue InputActionValue);

	// Simulates the given input action value for the given input action of a specific player controller until the simulation is ended with "EndSimulateControllerInputAction" (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);

	// Ends the simulation of the given input action. When this is called directly after "SimulateInputAction" the input will only be simulated for one tick.
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndSimulateInputAction(UObject* WorldContext, const UInputAction* InputAction);

	// Ends the simulation of the given input action for a specific player controller. When this is called directly after "SimulateControllerInputAction" the input will only be simulated for one tick.
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction);

public:
	// Simulates the given action with the given event. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyActionInput(UObject* WorldContext, const FName& ActionName, EInputEvent InputEventType);

	// Simulates the given action with the given event for a specific player controller. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event) (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	
	// Simulates the given axis action with the given amount. This will be active until it is reset by setting the value to 0 or calling "EndSimulateLegacyAxisInput". 
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName, float AxisValue);

	// Simulates the given axis action with the given amount for a specific player controller. This will be active until it is reset by setting the value to 0 or calling "EndSimulateLegacyAxisInput". (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);

	// Ends the simulation for the given axis action.
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndSimulateLegacyAxisInput(UObject* WorldContext, const FName& AxisName);

	// Ends the simulation for the given axis action for a specific player controller. (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName);
	
public:
	// Simulates the given key with the given event. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateKeyPressInput(UObject* WorldContext, const FName& PressedKey, EInputEvent InputEventType);

	// Simulates the given key with the given event for a specific player controller. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event) (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	
	// Simulates the given key with the given amount. This will be active until it is reset by setting the value to 0 or calling "EndSimulateKeyAxisInput". 
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void SimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey, float AxisValue);

	// Simulates the given axis with the given amount for a specific player controller. This will be active until it is reset by setting the value to 0 or calling "EndSimulateControllerKeyAxisInput". (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);

	// Ends the simulation for the given axis.
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndSimulateKeyAxisInput(UObject* WorldContext, const FName& AxisKey);

	// Ends the simulation for the given axis for a specific player controller. (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulation")
	static void EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey);

private:
	static class UFunKInputSimulationSystem* GetInputSimulationSystem(const UObject* WorldContext);
};

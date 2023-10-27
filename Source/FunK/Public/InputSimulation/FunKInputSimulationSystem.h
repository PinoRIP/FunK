// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "FunKInputSimulationSystem.generated.h"

class UInputAction;
class UInputSettings;

USTRUCT(BlueprintType)
struct FFunKInputActionSimulationTick
{
	GENERATED_BODY()

public:
	FFunKInputActionSimulationTick() {  }

	FInputActionValue Value;

	uint64 AddedInputTime = 0;
};

USTRUCT(BlueprintType)
struct FFunKAxisInputSimulationTick
{
	GENERATED_BODY()

public:
	FFunKAxisInputSimulationTick() {  }

	FKey Key;

	float AxisValue = 0.f;

	uint64 AddedInputTime = 0;
};

USTRUCT(BlueprintType)
struct FFunKPlayerControllerInputSimulations
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<const UInputAction*, FFunKInputActionSimulationTick> InputActionSimulations;
	
	UPROPERTY()
	TMap<FName, FFunKAxisInputSimulationTick> AxisInputSimulations;
};

/**
 * World subsystem used for input simulations.
 * FYI: The "Lower level" input simulation simulates key presses & can effect the enhanced input system / legacy input system
 * FYI2: The legacy input system is just an abstraction over key presses & can effect the enhanced input system / "Lower level" input
 */
UCLASS()
class FUNK_API UFunKInputSimulationSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	// Ends any kind of input simulation managed by this subsystem
	UFUNCTION(BlueprintCallable)
	void EndAllInputSimulations();

	// Disables the actual external inputs (so that only the simulated inputs count during a test)
	UFUNCTION(BlueprintCallable)
	void DisableActualInputs();

	// Re enables the actual external inputs when they have been disabled.
	UFUNCTION(BlueprintCallable)
	void EnableActualInputs();
	
public: //Enhanced input system

	// Simulates the given input action value for the given input action until the simulation is ended with "EndSimulateInputAction"
	UFUNCTION(BlueprintCallable)
	void SimulateInputAction(const UInputAction* InputAction, FInputActionValue InputActionValue);

	// Simulates the given input action value for the given input action of a specific player controller until the simulation is ended with "EndSimulateControllerInputAction" (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);

	// Ends the simulation of the given input action. When this is called directly after "SimulateInputAction" the input will only be simulated for one tick.
	UFUNCTION(BlueprintCallable)
	void EndSimulateInputAction(const UInputAction* InputAction);

	// Ends the simulation of the given input action for a specific player controller. When this is called directly after "SimulateControllerInputAction" the input will only be simulated for one tick.
	UFUNCTION(BlueprintCallable)
	void EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction);

private:
	FFunKInputActionSimulationTick& ScheduleInputActionSimulation(APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue);
	static void InjectInputForAction(const APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue);
	
public: //Legacy input system

	// Simulates the given action with the given event. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event)
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyActionInput(const FName& ActionName, EInputEvent InputEventType);

	// Simulates the given action with the given event for a specific player controller. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event) (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	// Simulates the given axis action with the given amount. This will be active until it is reset by setting the value to 0 or calling "EndSimulateLegacyAxisInput". 
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyAxisInput(const FName& AxisName, float AxisValue);

	// Simulates the given axis action with the given amount for a specific player controller. This will be active until it is reset by setting the value to 0 or calling "EndSimulateLegacyAxisInput". (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);

	// Ends the simulation for the given axis action.
	UFUNCTION(BlueprintCallable)
	void EndSimulateLegacyAxisInput(const FName& AxisName);

	// Ends the simulation for the given axis action for a specific player controller. (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName);

private:
	const FKey* GetInputActionKey(const FName& ActionName) const;
	const FKey* GetInputAxisKey(const FName& AxisName) const;
	static const UInputSettings* GetInputSettings();

public: //"Lower level" input system

	// Simulates the given key with the given event. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event)
	UFUNCTION(BlueprintCallable)
	void SimulateKeyPressInput(const FName& PressedKey, EInputEvent InputEventType);

	// Simulates the given key with the given event for a specific player controller. This will only be fired once. (Keep in mind that this fires only the event, meaning after "IE_Pressed" there will be no automatic "IE_Released" event) (Only really useful with splitscreen multiplayer) 
	UFUNCTION(BlueprintCallable)
	void SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	// Simulates the given key with the given amount. This will be active until it is reset by setting the value to 0 or calling "EndSimulateKeyAxisInput". 
	UFUNCTION(BlueprintCallable)
	void SimulateKeyAxisInput(const FName& AxisKey, float AxisValue);

	// Simulates the given axis with the given amount for a specific player controller. This will be active until it is reset by setting the value to 0 or calling "EndSimulateControllerKeyAxisInput". (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);

	// Ends the simulation for the given axis.
	UFUNCTION(BlueprintCallable)
	void EndSimulateKeyAxisInput(const FName& AxisKey);

	// Ends the simulation for the given axis for a specific player controller. (Only really useful with splitscreen multiplayer)
	UFUNCTION(BlueprintCallable)
	void EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey);
	
private:
	void SimulateAxisInput(APlayerController* PlayerController, const FKey& Key, float AxisValue);
	FFunKAxisInputSimulationTick& ScheduleAxisInputSimulation(APlayerController* PlayerController, const FKey& Key, float AxisValue);
	static void InjectAxisInput(APlayerController* PlayerController, const FKey& Key, float AxisValue, float DeltaTime);
	static float SenseAdjustAxisValue(const APlayerController* PlayerController, const FKey& Key, float AxisValue);
	static void SimulatePlayerControllerKeyPressInput(APlayerController* PlayerController, const FKey& Key, EInputEvent InputEventType);

private:
	TMap<APlayerController*, FFunKPlayerControllerInputSimulations> ControllerInputSimulations;

	FFunKPlayerControllerInputSimulations* GetControllerInputSimulations(APlayerController* PlayerController, bool bCreate = false);

	friend class UFunKInputSimulationBlueprintFunctionLibrary;
};

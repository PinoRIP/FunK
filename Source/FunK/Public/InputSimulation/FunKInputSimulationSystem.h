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
 * 
 */
UCLASS()
class FUNK_API UFunKInputSimulationSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable)
	void EndAllInputSimulations();

	UFUNCTION(BlueprintCallable)
	void DisableActualInputs();

	UFUNCTION(BlueprintCallable)
	void EnableActualInputs();
	
public: //Enhanced input system
	UFUNCTION(BlueprintCallable)
	void SimulateInputAction(const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateInputAction(const UInputAction* InputAction);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction);

private:
	FFunKInputActionSimulationTick& ScheduleInputActionSimulation(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);
	static void InjectInputForAction(const APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue);
	
public: //Legacy input system
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyActionInput(const FName& ActionName, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable)
	void SimulateLegacyAxisInput(const FName& AxisName, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateLegacyAxisInput(const FName& AxisName);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName);

private:
	const FKey* GetInputActionKey(const FName& ActionName) const;
	const FKey* GetInputAxisKey(const FName& AxisName) const;
	static const UInputSettings* GetInputSettings();

public: //"Lower level" input system
	UFUNCTION(BlueprintCallable)
	void SimulateKeyPressInput(const FName& PressedKey, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable)
	void SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable)
	void SimulateKeyAxisInput(const FName& AxisKey, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateKeyAxisInput(const FName& AxisKey);
	
	UFUNCTION(BlueprintCallable)
	void EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey);
	
private:
	void SimulateAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue);
	FFunKAxisInputSimulationTick& ScheduleAxisInputSimulation(APlayerController* PlayerController, const FKey& key, float AxisValue);
	static void InjectAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue, float DeltaTime);
	static float SenseAdjustAxisValue(const APlayerController* PlayerController, const FKey& key, float AxisValue);
	static void SimulatePlayerControllerKeyPressInput(APlayerController* PlayerController, const FKey& key, EInputEvent InputEventType);

private:
	TMap<APlayerController*, FFunKPlayerControllerInputSimulations> ControllerInputSimulations;

	FFunKPlayerControllerInputSimulations* GetControllerInputSimulations(APlayerController* PlayerController, bool create = false);

	friend class UFunKInputSimulationBlueprintFunctionLibrary;
};

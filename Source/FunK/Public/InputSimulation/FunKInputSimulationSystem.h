// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "FunKInputSimulationSystem.generated.h"

class UInputAction;
class UInputSettings;

USTRUCT(BlueprintType)
struct FFunKInputSimulationHandle
{
	GENERATED_BODY()

	FFunKInputSimulationHandle()
		: FFunKInputSimulationHandle(-1)
	{  }
	FFunKInputSimulationHandle(int32 key)
		: Key(key)
	{  }

	int32 Key;
};

USTRUCT(BlueprintType)
struct FFunKInputActionSimulationTick
{
	GENERATED_BODY()

public:
	FFunKInputActionSimulationTick() {  }

	UPROPERTY()
	const UInputAction* InputAction = nullptr;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	FInputActionValue Value;

	float ApplicationTimeMs = 0.f;

	float ActiveTimeMs = 0.f;
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
	
public: //Enhanced input system
	UFUNCTION(BlueprintCallable)
	void SimulateFirstPlayerInputActionOnce(const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateInputActionOnce(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);

	UFUNCTION(BlueprintCallable)
	void SimulateFirstPlayerInputActionFor(const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs);
	
	UFUNCTION(BlueprintCallable)
	void SimulateInputActionOnceFor(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs);

	UFUNCTION(BlueprintCallable)
	FFunKInputSimulationHandle SimulateFirstPlayerInputAction(const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	UFUNCTION(BlueprintCallable)
	FFunKInputSimulationHandle SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue);
	
	void EndAllInputActionSimulations();

private:
	TMap<int32, FFunKInputActionSimulationTick> InputActionSimulations;

	int32 SimulateFirstPlayerInputAction(const UInputAction* InputAction, const FInputActionValue& InputActionValue, float applicationTimeMs);
	int32 SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue, float applicationTimeMs);
	
	void EndInputActionSimulation(int32 key);
	
public: //Legacy input system
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyFirstPlayerActionInput(const FName& ActionName, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable)
	void SimulateLegacyFirstPlayerAxisInput(const FName& AxisName, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateLegacyAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue);

private:
	const FKey* GetInputActionKey(const FName& ActionName) const;
	const FKey* GetInputAxisKey(const FName& AxisName) const;
	static const UInputSettings* GetInputSettings();

public: //"Lower level" input system
	UFUNCTION(BlueprintCallable)
	void SimulateFirstPlayerKeyPressInput(const FName& PressedKey, EInputEvent InputEventType);
	
	UFUNCTION(BlueprintCallable)
	void SimulateKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType);

	UFUNCTION(BlueprintCallable)
	void SimulateFirstPlayerKeyAxisInput(const FName& AxisKey, float AxisValue);
	
	UFUNCTION(BlueprintCallable)
	void SimulateKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue);
	
private:
	static void SimulatePlayerControllerKeyPressInput(APlayerController* PlayerController, const FKey& key, EInputEvent InputEventType);
	static void SimulatePlayerControllerAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue);

	friend class UFunKInputSimulationBlueprintFunctionLibrary;
};

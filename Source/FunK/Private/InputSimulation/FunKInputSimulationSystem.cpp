// Fill out your copyright notice in the Description page of Project Settings.


#include "InputSimulation/FunKInputSimulationSystem.h"

#include "EnhancedInputSubsystems.h"
#include "FunKLogging.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"

/*void UFunKInputSimulationSystem::Do()
{
	UGameInstance* GameInst = GetWorld()->GetGameInstance();
	UGameViewportClient* ViewportClient = GameInst->GetGameViewportClient();
	FViewport* Viewport = ViewportClient->Viewport;

	int32 ControllerId = 0; // or whatever controller id, could be a function param
	FName PressedKey = FName(TEXT("X")); // or whatever key, could be a function param
	FInputKeyEventArgs Args = FInputKeyEventArgs(
			Viewport,
			ControllerId,
			FKey(PressedKey),
			EInputEvent::IE_Pressed);

	ViewportClient->InputKey(Args);
}*/

/*void UFunKInputSimulationSystem::Do()
{
	// Get the player controller that you care about from wherever you want
	APlayerController* PC;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	auto b = Subsystem->QueryKeysMappedToAction(nullptr);
 
	UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();

	Subsystem->
 
	UInputAction* InputAction = nullptr;
	InputAction->
	
	FInputActionValue ActionValue(1.0f); // This can be a bool, float, FVector2D, or FVector
	PlayerInput->InjectInputForAction(InputAction, ActionValue);

	PlayerInput->
}*/

void UFunKInputSimulationSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(InputActionSimulations.Num() > 0)
	{
		//TODO: This could be optimized
		for (TMap<int32, FFunKInputActionSimulationTick>::TIterator Iterator = InputActionSimulations.CreateIterator(); Iterator; ++Iterator)
		{
			auto& value = Iterator.Value();
			SimulateInputActionOnce(value.PlayerController, value.InputAction, value.Value);

			if(value.ApplicationTimeMs < 0)
			{
				value.ActiveTimeMs += DeltaTime;
				if(value.ActiveTimeMs > value.ApplicationTimeMs)
					Iterator.RemoveCurrent();
			}
		}
	}
}

void UFunKInputSimulationSystem::SimulateFirstPlayerInputActionOnce(const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	SimulateInputActionOnce(GetWorld()->GetFirstPlayerController(), InputAction, InputActionValue);
}

void UFunKInputSimulationSystem::SimulateInputActionOnce(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); 
	UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
	PlayerInput->InjectInputForAction(InputAction, InputActionValue);
}

void UFunKInputSimulationSystem::SimulateFirstPlayerInputActionFor(const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	SimulateFirstPlayerInputAction(InputAction, InputActionValue, applicationTimeMs);
}

void UFunKInputSimulationSystem::SimulateInputActionOnceFor(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue, float applicationTimeMs)
{
	SimulateInputAction(PlayerController, InputAction, InputActionValue, applicationTimeMs);
}

FFunKInputSimulationHandle UFunKInputSimulationSystem::SimulateFirstPlayerInputAction(const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	return FFunKInputSimulationHandle(SimulateFirstPlayerInputAction(InputAction, InputActionValue, -1));
}

FFunKInputSimulationHandle UFunKInputSimulationSystem::SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	return FFunKInputSimulationHandle(SimulateInputAction(PlayerController, InputAction, InputActionValue, -1));
}

int32 UFunKInputSimulationSystem::SimulateFirstPlayerInputAction(const UInputAction* InputAction, const FInputActionValue& InputActionValue, float applicationTimeMs)
{
	return SimulateInputAction(GetWorld()->GetFirstPlayerController(), InputAction, InputActionValue, applicationTimeMs);
}

int32 UFunKInputSimulationSystem::SimulateInputAction(APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue, float applicationTimeMs)
{
	static int32 key = 0;
	
	int32 localKey = key++;
	FFunKInputActionSimulationTick& entry = InputActionSimulations.Add(localKey, FFunKInputActionSimulationTick());
	entry.InputAction = InputAction;
	entry.PlayerController = PlayerController;
	entry.Value = InputActionValue;
	entry.ApplicationTimeMs = applicationTimeMs;

	return localKey;
}

void UFunKInputSimulationSystem::EndInputActionSimulation(int32 key)
{
	InputActionSimulations.Remove(key);
}

void UFunKInputSimulationSystem::EndAllInputActionSimulations()
{
	InputActionSimulations.Empty();
}

void UFunKInputSimulationSystem::SimulateLegacyFirstPlayerActionInput(const FName& ActionName, EInputEvent InputEventType)
{
	SimulateLegacyActionInput(GetWorld()->GetFirstPlayerController(), ActionName, InputEventType);
}

void UFunKInputSimulationSystem::SimulateLegacyActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}
	
	const FKey* InputAxisKey = GetInputActionKey(ActionName);
	if(!InputAxisKey) return;
	
}

void UFunKInputSimulationSystem::SimulateLegacyFirstPlayerAxisInput(const FName& AxisName, float AxisValue)
{
	SimulateLegacyAxisInput(GetWorld()->GetFirstPlayerController(), AxisName, AxisValue);
}

void UFunKInputSimulationSystem::SimulateLegacyAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}
	
	const FKey* InputAxisKey = GetInputAxisKey(AxisName);
	if(!InputAxisKey) return;

	SimulatePlayerControllerAxisInput(PlayerController, *InputAxisKey, AxisValue);
}

const FKey* UFunKInputSimulationSystem::GetInputActionKey(const FName& ActionName) const
{
	const UInputSettings* InputSettings = GetInputSettings();
	if (InputSettings) return nullptr;

	for (const FInputActionKeyMapping& Mapping : InputSettings->GetActionMappings())
	{
		if (Mapping.ActionName == ActionName)
		{
			return &Mapping.Key;
		}
	}

	UE_LOG(FunKLog, Error, TEXT("Input action key not found: %s"), *ActionName.ToString())
	return nullptr;
}

const FKey* UFunKInputSimulationSystem::GetInputAxisKey(const FName& AxisName) const
{
	const UInputSettings* InputSettings = GetInputSettings();
	if (InputSettings) return nullptr;

	for (const FInputAxisKeyMapping& Mapping : InputSettings->GetAxisMappings())
	{
		if (Mapping.AxisName == AxisName)
		{
			return &Mapping.Key;
		}
	}

	UE_LOG(FunKLog, Error, TEXT("Input axis key not found: %s"), *AxisName.ToString())
	return nullptr;
}

const UInputSettings* UFunKInputSimulationSystem::GetInputSettings()
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();
	if(!InputSettings)
	{
		UE_LOG(FunKLog, Error, TEXT("Input settings not found!"))
	}

	return InputSettings;
}

void UFunKInputSimulationSystem::SimulateFirstPlayerKeyPressInput(const FName& PressedKey, EInputEvent InputEventType)
{
	SimulateKeyPressInput(GetWorld()->GetFirstPlayerController(), PressedKey, InputEventType);
}

void UFunKInputSimulationSystem::SimulateKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}
	
	SimulatePlayerControllerKeyPressInput(PlayerController, FKey(PressedKey), InputEventType);
}

void UFunKInputSimulationSystem::SimulateFirstPlayerKeyAxisInput(const FName& AxisKey, float AxisValue)
{
	SimulateKeyAxisInput(GetWorld()->GetFirstPlayerController(), AxisKey, AxisValue);
}

void UFunKInputSimulationSystem::SimulateKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}
	
	SimulatePlayerControllerAxisInput(PlayerController, FKey(AxisKey), AxisValue);
}

void UFunKInputSimulationSystem::SimulatePlayerControllerKeyPressInput(APlayerController* PlayerController, const FKey& key, EInputEvent InputEventType)
{
	PlayerController->InputKey(FInputKeyParams(key, InputEventType, 1));
}

void UFunKInputSimulationSystem::SimulatePlayerControllerAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue)
{
	PlayerController->InputKey(FInputKeyParams(key, FVector(AxisValue, 0.0, 0.0), 0.f, 1));
}

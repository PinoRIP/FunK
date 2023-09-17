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

	if(ControllerInputSimulations.Num() > 0)
	{
		for (TTuple<APlayerController*, FFunKPlayerControllerInputSimulations>& ControllerInputSimulation : ControllerInputSimulations)
		{
			for (TTuple<const UInputAction*, FFunKInputActionSimulationTick>& InputActionSimulation : ControllerInputSimulation.Value.InputActionSimulations)
			{
				if(InputActionSimulation.Value.AddedInputTime == GInputTime) continue;
				InjectInputForAction(ControllerInputSimulation.Key, InputActionSimulation.Key, InputActionSimulation.Value.Value);
			}
			
			for (TTuple<FName, FFunKAxisInputSimulationTick>& AxisInputSimulation : ControllerInputSimulation.Value.AxisInputSimulations)
			{
				if(AxisInputSimulation.Value.AddedInputTime == GInputTime) continue;
				InjectAxisInput(ControllerInputSimulation.Key, AxisInputSimulation.Value.Key, AxisInputSimulation.Value.AxisValue, DeltaTime);
			}
		}
	}
}

TStatId UFunKInputSimulationSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFunKInputSimulationSystem, STATGROUP_Tickables);
}

void UFunKInputSimulationSystem::EndAllInputSimulations()
{
	ControllerInputSimulations.Empty();
}

void UFunKInputSimulationSystem::SimulateInputAction(const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	SimulateControllerInputAction(GetWorld()->GetFirstPlayerController(), InputAction, InputActionValue);
}

void UFunKInputSimulationSystem::SimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}

	ScheduleInputActionSimulation(PlayerController, InputAction, InputActionValue);
	InjectInputForAction(PlayerController, InputAction, InputActionValue);
}

void UFunKInputSimulationSystem::EndSimulateInputAction(const UInputAction* InputAction)
{
	EndSimulateControllerInputAction(GetWorld()->GetFirstPlayerController(), InputAction);
}

void UFunKInputSimulationSystem::EndSimulateControllerInputAction(APlayerController* PlayerController, const UInputAction* InputAction)
{
	FFunKPlayerControllerInputSimulations* InputSimulations = GetControllerInputSimulations(PlayerController);
	if (!InputSimulations) return;

	InputSimulations->InputActionSimulations.Remove(InputAction);

	if (InputSimulations->InputActionSimulations.Num() <= 0 && InputSimulations->AxisInputSimulations.Num())
		ControllerInputSimulations.Remove(PlayerController);
}

FFunKInputActionSimulationTick& UFunKInputSimulationSystem::ScheduleInputActionSimulation(APlayerController* PlayerController, const UInputAction* InputAction, FInputActionValue InputActionValue)
{
	FFunKPlayerControllerInputSimulations* InputSimulations = GetControllerInputSimulations(PlayerController, true);
	FFunKInputActionSimulationTick* InputActionEntry = InputSimulations->InputActionSimulations.Find(InputAction);
	if(!InputActionEntry)
	{
		InputActionEntry = &InputSimulations->InputActionSimulations.Add(InputAction, FFunKInputActionSimulationTick());
	}

	InputActionEntry->Value = InputActionValue;
	InputActionEntry->AddedInputTime = GInputTime;

	return *InputActionEntry;
}

void UFunKInputSimulationSystem::InjectInputForAction(const APlayerController* PlayerController, const UInputAction* InputAction, const FInputActionValue& InputActionValue)
{
	const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); 
	UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
	PlayerInput->InjectInputForAction(InputAction, InputActionValue);
}

void UFunKInputSimulationSystem::SimulateLegacyActionInput(const FName& ActionName, EInputEvent InputEventType)
{
	SimulateLegacyControllerActionInput(GetWorld()->GetFirstPlayerController(), ActionName, InputEventType);
}

void UFunKInputSimulationSystem::SimulateLegacyControllerActionInput(APlayerController* PlayerController, const FName& ActionName, EInputEvent InputEventType)
{
	const FKey* InputAxisKey = GetInputActionKey(ActionName);
	if(!InputAxisKey) return;

	SimulatePlayerControllerKeyPressInput(PlayerController, *InputAxisKey, InputEventType);
}

void UFunKInputSimulationSystem::SimulateLegacyAxisInput(const FName& AxisName, float AxisValue)
{
	SimulateLegacyControllerAxisInput(GetWorld()->GetFirstPlayerController(), AxisName, AxisValue);
}

void UFunKInputSimulationSystem::SimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName, float AxisValue)
{
	const FKey* InputAxisKey = GetInputAxisKey(AxisName);
	if(!InputAxisKey) return;
	
	SimulateAxisInput(PlayerController, *InputAxisKey, AxisValue);
}

void UFunKInputSimulationSystem::EndSimulateLegacyAxisInput(const FName& AxisName)
{
	EndSimulateLegacyControllerAxisInput(GetWorld()->GetFirstPlayerController(), AxisName);
}

void UFunKInputSimulationSystem::EndSimulateLegacyControllerAxisInput(APlayerController* PlayerController, const FName& AxisName)
{
	const FKey* InputAxisKey = GetInputAxisKey(AxisName);
	if(!InputAxisKey) return;

	EndSimulateControllerKeyAxisInput(PlayerController, InputAxisKey->GetFName());
}

const FKey* UFunKInputSimulationSystem::GetInputActionKey(const FName& ActionName) const
{
	const UInputSettings* InputSettings = GetInputSettings();
	if (!InputSettings) return nullptr;

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
	if (!InputSettings) return nullptr;

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

void UFunKInputSimulationSystem::SimulateKeyPressInput(const FName& PressedKey, EInputEvent InputEventType)
{
	SimulateControllerKeyPressInput(GetWorld()->GetFirstPlayerController(), PressedKey, InputEventType);
}

void UFunKInputSimulationSystem::SimulateControllerKeyPressInput(APlayerController* PlayerController, const FName& PressedKey, EInputEvent InputEventType)
{
	SimulatePlayerControllerKeyPressInput(PlayerController, FKey(PressedKey), InputEventType);
}

void UFunKInputSimulationSystem::SimulateKeyAxisInput(const FName& AxisKey, float AxisValue)
{
	SimulateControllerKeyAxisInput(GetWorld()->GetFirstPlayerController(), AxisKey, AxisValue);
}

void UFunKInputSimulationSystem::SimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey, float AxisValue)
{
	SimulateAxisInput(PlayerController, FKey(AxisKey), AxisValue);
}

void UFunKInputSimulationSystem::EndSimulateKeyAxisInput(const FName& AxisKey)
{
	EndSimulateControllerKeyAxisInput(GetWorld()->GetFirstPlayerController(), AxisKey);
}

void UFunKInputSimulationSystem::EndSimulateControllerKeyAxisInput(APlayerController* PlayerController, const FName& AxisKey)
{
	FFunKPlayerControllerInputSimulations* InputSimulations = GetControllerInputSimulations(PlayerController);
	if (!InputSimulations) return;

	InputSimulations->AxisInputSimulations.Remove(AxisKey);

	if (InputSimulations->InputActionSimulations.Num() <= 0 && InputSimulations->AxisInputSimulations.Num())
		ControllerInputSimulations.Remove(PlayerController);
}

void UFunKInputSimulationSystem::SimulatePlayerControllerKeyPressInput(APlayerController* PlayerController, const FKey& key, EInputEvent InputEventType)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}
	
	PlayerController->InputKey(FInputKeyParams(key, InputEventType, 1));
}

void UFunKInputSimulationSystem::SimulateAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue)
{
	if(!PlayerController)
	{
		UE_LOG(FunKLog, Error, TEXT("PlayerController not found!"))
		return;
	}

	if(AxisValue == 0.f)
	{
		EndSimulateControllerKeyAxisInput(PlayerController, key.GetFName());
		return;
	}

	const FFunKAxisInputSimulationTick& entry = ScheduleAxisInputSimulation(PlayerController, key, AxisValue);
	InjectAxisInput(PlayerController, entry.Key, entry.AxisValue, 0.f);
}

FFunKAxisInputSimulationTick& UFunKInputSimulationSystem::ScheduleAxisInputSimulation(APlayerController* PlayerController, const FKey& key, float AxisValue)
{
	FFunKPlayerControllerInputSimulations* InputSimulations = GetControllerInputSimulations(PlayerController, true);
	FFunKAxisInputSimulationTick* AxisInputEntry = InputSimulations->AxisInputSimulations.Find(key.GetFName());
	if(!AxisInputEntry)
	{
		AxisInputEntry = &InputSimulations->AxisInputSimulations.Add(key.GetFName(), FFunKAxisInputSimulationTick());
	}

	AxisInputEntry->Key = key;
	AxisInputEntry->AxisValue = SenseAdjustAxisValue(PlayerController, key, AxisValue);
	AxisInputEntry->AddedInputTime = GInputTime;

	return *AxisInputEntry;
}

void UFunKInputSimulationSystem::InjectAxisInput(APlayerController* PlayerController, const FKey& key, float AxisValue, float DeltaTime)
{
	PlayerController->InputKey(FInputKeyParams(key, FVector(AxisValue, 0.0, 0.0), DeltaTime, 1));
}

float UFunKInputSimulationSystem::SenseAdjustAxisValue(const APlayerController* PlayerController, const FKey& key, float AxisValue)
{
	FInputAxisProperties AxisProps;
	if (PlayerController->PlayerInput->GetAxisProperties(key, AxisProps) && AxisProps.Sensitivity != 1.f)
	{
		return AxisValue / AxisProps.Sensitivity;
	}
	
	return AxisValue;
}

FFunKPlayerControllerInputSimulations* UFunKInputSimulationSystem::GetControllerInputSimulations(APlayerController* PlayerController, bool create)
{
	FFunKPlayerControllerInputSimulations* Entry = ControllerInputSimulations.Find(PlayerController);
	if (Entry || !create) return Entry;
	return &ControllerInputSimulations.Add(PlayerController, FFunKPlayerControllerInputSimulations());
}

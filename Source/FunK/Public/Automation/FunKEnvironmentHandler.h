// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FunKTestInstructions.h"
#include "FunKEnvironmentHandler.generated.h"

class AFunKWorldTestController;
class UFunKWorldSubsystem;
class UFunKEventBusSubsystem;

UENUM()
enum class EFunKEnvironmentWorldState
{
	IsRunning,
	IsStarting,
	CantStart
};

// This class does not need to be modified.
UINTERFACE()
class UFunKEnvironmentHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FUNK_API IFunKEnvironmentHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual EFunKEnvironmentWorldState UpdateWorldState(const FFunKTestInstructions& Instructions) = 0;
	
	virtual UWorld* GetWorld() = 0;

	UFunKEventBusSubsystem* GetEventBus();
	UFunKWorldSubsystem* GetWorldSubsystem();
	AFunKWorldTestController* GetTestController();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/EventBus/FunKEventBusSubsystem.h"
#include "FunKEventBusRegistrationContainer.generated.h"

/*
 * Container that holds multiple registrations.
 */
USTRUCT()
struct FFunKEventBusRegistrationContainer
{
	GENERATED_BODY()

public:
	// Unregisters all registrations
	void Unregister();

	// Adds a registration to the container
	void Add(const FFunKEventBusRegistration& Registration);

private:
	TArray<FFunKEventBusRegistration> Registrations;
};

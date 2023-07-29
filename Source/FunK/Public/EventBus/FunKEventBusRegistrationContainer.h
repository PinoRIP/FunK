// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEventBusSubsystem.h"
#include "FunKEventBusRegistrationContainer.generated.h"

USTRUCT()
struct FFunKEventBusRegistrationContainer
{
	GENERATED_BODY()

public:
	void Unregister();
	void Add(const FFunKEventBusRegistration& Registration);

private:
	TArray<FFunKEventBusRegistration> Registrations;
};

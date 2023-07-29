// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/FunKEnvironmentHandler.h"


// Add default functionality here for any IFunKEnvironmentHandler functions that are not pure virtual.
UFunKEventBusSubsystem* IFunKEnvironmentHandler::GetEventBus()
{
	const UWorld* World = GetWorld();
	if(!World) return nullptr;
	return World->GetSubsystem<UFunKEventBusSubsystem>();
}

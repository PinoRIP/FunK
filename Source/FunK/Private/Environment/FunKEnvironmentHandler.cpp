// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/FunKEnvironmentHandler.h"
#include "FunKWorldSubsystem.h"

UFunKEventBusSubsystem* IFunKEnvironmentHandler::GetEventBus()
{
	const UWorld* World = GetWorld();
	if(!World) return nullptr;
	return World->GetSubsystem<UFunKEventBusSubsystem>();
}

UFunKWorldSubsystem* IFunKEnvironmentHandler::GetWorldSubsystem()
{
	const UWorld* World = GetWorld();
	if(!World) return nullptr;
	return World->GetSubsystem<UFunKWorldSubsystem>();
}

AFunKWorldTestController* IFunKEnvironmentHandler::GetTestController()
{
	UFunKWorldSubsystem* WorldSubsystem = GetWorldSubsystem();
	if(!WorldSubsystem) return nullptr;
	return WorldSubsystem->GetLocalTestController();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldSubsystem.h"

#include "FunKEngineSubsystem.h"
#include "FunKSettingsObject.h"
#include "FunKWorldTestController.h"
#include "EventBus/FunKEventBusReplicationController.h"

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if(!LocalTestController)
	{
		LocalTestController = NewTestController();
	}
	
	return LocalTestController;
}

int32 UFunKWorldSubsystem::GetPeerIndex() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode < NM_Client) return 0;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return INDEX_NONE;

	const AFunKEventBusReplicationController* funkEventController = funkEventSubsystem->GetLocalController();
	if(!funkEventController) return INDEX_NONE;

	return funkEventController->GetControllerNumber();
}

int32 UFunKWorldSubsystem::GetPeerCount() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();
	if(netMode == NM_Standalone) return 1;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return 1;

	return funkEventSubsystem->GetReplicationControllerCount() + 1;
}

bool UFunKWorldSubsystem::IsServerDedicated() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode < NM_Client) return netMode == NM_DedicatedServer;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return false;

	const AFunKEventBusReplicationController* funkEventController = funkEventSubsystem->GetLocalController();
	if(!funkEventController) return false;

	return funkEventController->GetIsServerDedicated();
}

AFunKWorldTestController* UFunKWorldSubsystem::NewTestController() const
{
	UClass* ReplicatedManagerClass = GetDefault<UFunKSettingsObject>()->Settings.WorldTestControllerClassOverride.Get();
	return ReplicatedManagerClass
		? GetWorld()->SpawnActor<AFunKWorldTestController>(ReplicatedManagerClass)
		: GetWorld()->SpawnActor<AFunKWorldTestController>();
}

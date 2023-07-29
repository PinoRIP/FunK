// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldSubsystem.h"

#include "FunKEngineSubsystem.h"
#include "FunKSettingsObject.h"
#include "FunKWorldTestController.h"
#include "EventBus/FunKEventBusReplicationController.h"

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if(!LocalTestController && GetWorld()->GetNetMode() != NM_Client)
	{
		LocalTestController = NewTestController();
	}
	
	return LocalTestController;
}

void UFunKWorldSubsystem::SetLocalTestController(AFunKWorldTestController* localTestController)
{
	if(!LocalTestController && GetWorld()->GetNetMode() == NM_Client)
	{
		LocalTestController = localTestController;
	}
}

bool UFunKWorldSubsystem::HasLocalTestController() const
{
	return !!LocalTestController;
}

void UFunKWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if(GEngine)
	{
		UFunKEngineSubsystem* funk = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>();
		if(funk && funk->IsRunning())
		{
			funk->CallbackTestWorldBeganPlay(GetWorld());
		}
	}
}

void UFunKWorldSubsystem::Deinitialize()
{
	Registration.Unregister();
	Super::Deinitialize();
}

int32 UFunKWorldSubsystem::GetRoleNum() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode > NM_Client) return 0;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return INDEX_NONE;

	const AFunKEventBusReplicationController* funkEventController = funkEventSubsystem->GetLocalController();
	if(!funkEventController) return INDEX_NONE;

	return funkEventController->GetControllerNumber();
}

bool UFunKWorldSubsystem::IsServerDedicated() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode > NM_Client) return netMode == NM_DedicatedServer;

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

// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus/FunKEventBusSubsystem.h"
#include "EventBus/FunKEventBusReplicationController.h"
#include "GameFramework/GameModeBase.h"

bool FFunKEventBusMessage::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Type;

	if (Type)
	{
		if (Ar.IsLoading())
		{
			auto * NewData = FMemory::Malloc(Type->GetStructureSize());
			Type->InitializeStruct(NewData);

			Instance = TSharedPtr<void>(NewData);
		}

		if (Type->StructFlags & STRUCT_NetSerializeNative)
		{
			Type->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, Instance.Get());
		}
		else
		{
			// This won't work since FStructProperty::NetSerializeItem is deprecrated.
			//	1) we have to manually crawl through the topmost struct's fields since we don't have a FStructProperty for it (just the UScriptProperty)
			//	2) if there are any UStructProperties in the topmost struct's fields, we will assert in FStructProperty::NetSerializeItem.

			auto ptr = Instance.Get();
			for (TFieldIterator<FProperty> It(Type); It; ++It)
			{
				if (It->PropertyFlags & CPF_RepSkip)
				{
					continue;
				}

				void* PropertyData = It->ContainerPtrToValuePtr<void*>(ptr);

				It->NetSerializeItem(Ar, Map, PropertyData);
			}
		}

		return true;
	}
	else 
	{
		Ar.SetError();
		return false;
	}
}

void FFunKEventBusRegistration::Unregister()
{
	if(IsBasicValid())
	{
		Subsystem->Unregister(Key);
		Key = INDEX_NONE;
	}
}

bool FFunKEventBusRegistration::IsValid() const
{
	return IsBasicValid() && Subsystem->HasHandler(Key);
}

bool FFunKEventBusRegistration::IsBasicValid() const
{
	return Subsystem && Key > INDEX_NONE;
}

void UFunKEventBusSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const ENetMode netMode = InWorld.GetNetMode();
	if(netMode == NM_ListenServer || netMode == NM_DedicatedServer)
	{
		FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UFunKEventBusSubsystem::OnConnect);
		FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &UFunKEventBusSubsystem::OnDisconnect);

		for (FConstPlayerControllerIterator Iterator = InWorld.GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			RegisterController(Iterator->Get());
		}
	}
}

void UFunKEventBusSubsystem::RegisterLocalReplicationController(AFunKEventBusReplicationController* localController)
{
	if(!localController)
		return;
	
	LocalController = localController;	
}

bool UFunKEventBusSubsystem::HasLocalReplicationController() const
{
	return !!LocalController;
}

void UFunKEventBusSubsystem::ReplicationControllerReady(AFunKEventBusReplicationController* controller)
{
	for (FReplicationControllerState& Controller : ReplicationControllers)
	{
		if(Controller.Reference == controller)
			Controller.IsReady = true;
	}

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::ReceiveMessage(const FFunKEventBusMessage& Message)
{
	const FFunKEventBusMessage& event = LastEvents.Add(Message.Type->GetName(), Message);

	TArray<int32> keys;
	Handlers.GetKeys(keys);
	
	for (int32 key : keys)
	{
		FFunKEventBusEventHandler& Handler = Handlers[key];
		Handler(event);
	}
}

void UFunKEventBusSubsystem::OnConnect(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	RegisterController(NewPlayer);
}

void UFunKEventBusSubsystem::OnDisconnect(AGameModeBase* GameMode, AController* Controller)
{
	DeregisterController(Controller);
}

void UFunKEventBusSubsystem::UpdateControllerStats()
{
	FFunKEventBusControllerEvent controllerEvent;
	controllerEvent.Controllers = ReplicationControllers.Num();
	const bool isDedicatedServer = GetWorld()->GetNetMode() == NM_DedicatedServer;

	for (const FReplicationControllerState& Controller : ReplicationControllers)
	{
		if(Controller.IsReady)
			controllerEvent.ControllersReady++;
	}

	for (int i = 0; i < controllerEvent.Controllers; ++i)
	{
		const FReplicationControllerState& Controller = ReplicationControllers[i];
		
		Controller.Reference->ActiveController = controllerEvent.ControllersReady;
		Controller.Reference->IsServerDedicated = isDedicatedServer;
		Controller.Reference->ControllerNumber = i + 1;
	}

	Raise(controllerEvent);
}

void UFunKEventBusSubsystem::RegisterController(APlayerController* NewPlayer)
{
	if(!NewPlayer)
		return;
	
	if(NewPlayer->IsLocalController())
		return;
	
	for (const FReplicationControllerState& ReplicationController : ReplicationControllers)
	{
		if(NewPlayer == ReplicationController.Reference->GetOwner())
			return;
	}

	const auto replicationController = NewPlayer->GetWorld()->SpawnActor<AFunKEventBusReplicationController>();
	replicationController->SetOwner(NewPlayer);
	FReplicationControllerState& state = ReplicationControllers.Add_GetRef(FReplicationControllerState());
	state.Reference = replicationController;

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::DeregisterController(AController* Controller)
{
	for (int i = 0; i < ReplicationControllers.Num(); ++i)
	{
		FReplicationControllerState& ReplicationController = ReplicationControllers[i];
		if(ReplicationController.Reference->GetOwner() == Controller)
		{
			ReplicationController.Reference->GetWorld()->RemoveActor(ReplicationController.Reference, false);
			ReplicationControllers.RemoveAt(i);
			break;
		}
	}

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::Unregister(int32 key)
{
	Handlers.Remove(key);
}

bool UFunKEventBusSubsystem::HasHandler(int32 key)
{
	return !!Handlers.Find(key);
}

void UFunKEventBusSubsystem::SendMessage(const FFunKEventBusMessage& Message)
{
	auto netMode = GetWorld()->GetNetMode();
	if(netMode == NM_Client)
	{
		if(LocalController)
			LocalController->ServerSendMessage(Message);
	}
	else if(netMode != NM_Standalone)
	{
		for (FReplicationControllerState& ReplicationController : ReplicationControllers)
		{
			ReplicationController.Reference->ClientSendMessage(Message);
		}
	}

	ReceiveMessage(Message);
}

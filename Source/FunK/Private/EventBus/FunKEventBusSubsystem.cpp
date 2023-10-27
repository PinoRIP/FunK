// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus/FunKEventBusSubsystem.h"
#include "EventBus/FunKEventBusReplicationController.h"
#include "GameFramework/GameModeBase.h"

bool FFunKEventBusMessage::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << CallbackId;
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
			//TODO: Make this work in all cases. FGuid is not supported...
			// This won't work since FStructProperty::NetSerializeItem is deprecrated.
			//	1) we have to manually crawl through the topmost struct's fields since we don't have a FStructProperty for it (just the UScriptProperty)
			//	2) if there are any UStructProperties in the topmost struct's fields, we will assert in FStructProperty::NetSerializeItem.

			void* Ptr = Instance.Get();
			for (TFieldIterator<FProperty> It(Type); It; ++It)
			{
				if (It->PropertyFlags & CPF_RepSkip)
				{
					continue;
				}

				void* PropertyData = It->ContainerPtrToValuePtr<void*>(Ptr);
				
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
	if (IsBasicValid())
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

	const ENetMode NetMode = InWorld.GetNetMode();
	if (NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
	{
		FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UFunKEventBusSubsystem::OnConnect);
		FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &UFunKEventBusSubsystem::OnDisconnect);

		FConstPlayerControllerIterator Iterator = InWorld.GetPlayerControllerIterator();
		if (NetMode == NM_ListenServer)
			++Iterator; // Playable server doesn't need a replication controller

		for (; Iterator; ++Iterator)
		{
			RegisterController(Iterator->Get());
		}
	}
}

void UFunKEventBusSubsystem::RegisterLocalReplicationController(AFunKEventBusReplicationController* Controller)
{
	if (!Controller)
		return;
	
	LocalController = Controller;	
}

bool UFunKEventBusSubsystem::AnyHandler() const
{
	return Handlers.Num() > 0;
}

bool UFunKEventBusSubsystem::HasLocalReplicationController() const
{
	return !!LocalController;
}

void UFunKEventBusSubsystem::ReplicationControllerReady(const AFunKEventBusReplicationController* Controller)
{
	for (FReplicationControllerState& ReplicationControllerState : ReplicationControllers)
	{
		if (ReplicationControllerState.Reference == Controller)
			ReplicationControllerState.IsReady = true;
	}

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::ReceiveMessage(const FFunKEventBusMessage& Message)
{
	const FFunKEventBusMessage& Event = LastEvents.Add(Message.Type->GetName(), Message);

	TArray<int32> Keys;
	Handlers.GetKeys(Keys);

	for (int i = Keys.Num() - 1; i >= 0; --i)
	{
		const TFunction<void(const FFunKEventBusMessage&)>* HandlerPtr = Handlers.Find(Keys[i]);
		if (!HandlerPtr)
			continue;
		
		(*HandlerPtr)(Event);
	}
}

void UFunKEventBusSubsystem::CheckCallback(FGuid CallbackId, AFunKEventBusReplicationController* FromController)
{
	if (!CallbackId.IsValid())
		return;
	
	FFunKEventCallbackState* CallbackState = Callbacks.Find(CallbackId);
	if (!CallbackState)
		return;

	if (FromController)
		CallbackState->DispatchedControllers.Remove(FromController);

	if (CallbackState->DispatchedControllers.Num() == 0)
	{
		CallbackState->Function();
		Callbacks.Remove(CallbackId);
	}
}

int32 UFunKEventBusSubsystem::GetReplicationControllerCount() const
{
	if (GetWorld()->GetNetMode() < NM_Client)
		return ReplicationControllers.Num();

	return GetLocalController()->GetControllerNumber();
}

int32 UFunKEventBusSubsystem::GetReadyReplicationControllerCount() const
{
	if (GetWorld()->GetNetMode() < NM_Client)
	{
		int32 Counter = 0;
		for (const FReplicationControllerState& ReplicationController : ReplicationControllers)
		{
			if (ReplicationController.IsReady)
				Counter++;
		}

		return Counter;
	}

	return GetLocalController()->GetActiveController();
}

void UFunKEventBusSubsystem::GetReplicationControllers(TArray<AFunKEventBusReplicationController*>& Array)
{
	Array.Empty(ReplicationControllers.Num());
	for (FReplicationControllerState& ReplicationController : ReplicationControllers)
	{
		Array.Add(ReplicationController.Reference);
	}
}

void UFunKEventBusSubsystem::OnConnect(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	RegisterController(NewPlayer);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UFunKEventBusSubsystem::OnDisconnect(AGameModeBase* GameMode, AController* Controller)
{
	DeregisterController(Controller);
}

void UFunKEventBusSubsystem::UpdateControllerStats()
{
	FFunKEventBusControllerEvent ControllerEvent;
	ControllerEvent.Controllers = ReplicationControllers.Num();
	const bool IsDedicatedServer = GetWorld()->GetNetMode() == NM_DedicatedServer;

	for (const FReplicationControllerState& Controller : ReplicationControllers)
	{
		if (Controller.IsReady)
			ControllerEvent.ControllersReady++;
	}

	for (int i = 0; i < ControllerEvent.Controllers; ++i)
	{
		const FReplicationControllerState& Controller = ReplicationControllers[i];
		
		Controller.Reference->ActiveController = ControllerEvent.ControllersReady;
		Controller.Reference->IsServerDedicated = IsDedicatedServer;
		Controller.Reference->ControllerNumber = i + 1;
	}

	Raise(ControllerEvent);
}

void UFunKEventBusSubsystem::RegisterController(APlayerController* NewPlayer)
{
	if (!NewPlayer)
		return;
	
	if (NewPlayer->IsLocalController())
		return;
	
	for (const FReplicationControllerState& ReplicationController : ReplicationControllers)
	{
		if (NewPlayer == ReplicationController.Reference->GetOwner())
			return;
	}

	const auto ReplicationController = NewPlayer->GetWorld()->SpawnActor<AFunKEventBusReplicationController>();
	ReplicationController->SetOwner(NewPlayer);
	
	FReplicationControllerState& State = ReplicationControllers.Add_GetRef(FReplicationControllerState());
	State.Reference = ReplicationController;

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::DeregisterController(const AController* Controller)
{
	for (int i = 0; i < ReplicationControllers.Num(); ++i)
	{
		const FReplicationControllerState& ReplicationControllerToRemove = ReplicationControllers[i];
		if (ReplicationControllerToRemove.Reference->GetOwner() == Controller)
		{
			for (const FReplicationControllerState& ReplicationControllerToNotify : ReplicationControllers)
			{
				if (ReplicationControllerToNotify.Reference != ReplicationControllerToRemove.Reference)
				{
					ReplicationControllerToNotify.Reference->NotifyReplicationControllerRemoved(ReplicationControllerToRemove.Reference);
				}
			}
			
			ReplicationControllerToRemove.Reference->GetWorld()->RemoveActor(ReplicationControllerToRemove.Reference, false);
			ReplicationControllers.RemoveAt(i);
			break;
		}
	}

	UpdateControllerStats();
}

void UFunKEventBusSubsystem::Unregister(const int32 Key)
{
	Handlers.Remove(Key);
}

bool UFunKEventBusSubsystem::HasHandler(const int32 Key)
{
	return !!Handlers.Find(Key);
}

void UFunKEventBusSubsystem::SendMessage(FFunKEventBusMessage& Message, TOptional<TFunction<void()>>& Callback)
{
	FFunKEventCallbackState* CallbackState = nullptr;
	if (Callback.IsSet())
	{
		FGuid ID = FGuid::NewGuid();
		CallbackState = &Callbacks.Emplace(ID);
		CallbackState->Id = ID;
		CallbackState->Function = Callback.GetValue();

		Message.CallbackId = ID;
	}

	const ENetMode NetMode = GetWorld()->GetNetMode();
	if (NetMode == NM_Client)
	{
		if (LocalController)
		{
			LocalController->ServerSendMessage(Message);

			if (CallbackState)
				CallbackState->DispatchedControllers.Add(LocalController);
		}
	}
	else if (NetMode != NM_Standalone)
	{
		for (FReplicationControllerState& ReplicationController : ReplicationControllers)
		{
			ReplicationController.Reference->ClientSendMessage(Message);

			if (CallbackState)
				CallbackState->DispatchedControllers.Add(ReplicationController.Reference);
		}
	}

	ReceiveMessage(Message);

	if (CallbackState)
		CheckCallback(CallbackState->Id, nullptr);
}

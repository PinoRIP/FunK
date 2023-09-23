// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/EventBus/FunKEventBusReplicationController.h"
#include "GameFramework/PlayerController.h"
#include "Internal/EventBus/FunKEventBusSubsystem.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFunKEventBusReplicationController::AFunKEventBusReplicationController()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AFunKEventBusReplicationController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( AFunKEventBusReplicationController, ActiveController );
	DOREPLIFETIME( AFunKEventBusReplicationController, ControllerNumber );
	DOREPLIFETIME( AFunKEventBusReplicationController, IsServerDedicated );
}

bool AFunKEventBusReplicationController::IsControllerLocallyReady() const
{
	return HasActorBegunPlay() && GetWorld()->GetSubsystem<UFunKEventBusSubsystem>()->HasLocalReplicationController() && (GetNetMode() != NM_Client || GetOwner() == GetWorld()->GetFirstPlayerController());
}

void AFunKEventBusReplicationController::CheckControllerReady()
{
	if(IsControllerLocallyReady() && !IsControllerReadinessSend)
	{
		IsControllerReadinessSend = true;
		ServerControllerReady();
	}
}

// Called when the game starts or when spawned
void AFunKEventBusReplicationController::BeginPlay()
{
	Super::BeginPlay();

	const ENetMode netMode = GetNetMode();
	if(netMode == NM_Client)
	{
		CheckControllerReady();
	}
}

void AFunKEventBusReplicationController::ServerEventCallback_Implementation(FGuid callbackId)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		TArray<AFunKEventBusReplicationController*> ReplicationControllers;
		Subsystem->GetReplicationControllers(ReplicationControllers);

		for (AFunKEventBusReplicationController* ReplicationController : ReplicationControllers)
		{
			if(ReplicationController->ReportProxiedCallbackResponse(this, callbackId))
				return;
		}

		Subsystem->CheckCallback(callbackId, this);
	}
}

void AFunKEventBusReplicationController::ClientEventCallback_Implementation(FGuid callbackId)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->CheckCallback(callbackId, this);
	}
}

void AFunKEventBusReplicationController::ServerControllerReady_Implementation()
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReplicationControllerReady(this);
	}
}

void AFunKEventBusReplicationController::OnRep_Owner()
{
	Super::OnRep_Owner();

	if(GetNetMode() == NM_Client && GetOwner() == GetWorld()->GetFirstPlayerController())
	{
		if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
		{
			Subsystem->RegisterLocalReplicationController(this);
			CheckControllerReady();
		}
	}
}

void AFunKEventBusReplicationController::NotifyReplicationControllerRemoved(AFunKEventBusReplicationController* ReplicationController)
{
	for (const TTuple<FGuid, FFunKCallbackProxyState>& ProxiedCallback : ProxiedCallbacks)
	{
		ReportProxiedCallbackResponse(ReplicationController, ProxiedCallback.Key);
	}
}

bool AFunKEventBusReplicationController::ReportProxiedCallbackResponse(AFunKEventBusReplicationController* ReplicationController, FGuid callbackId)
{
	FFunKCallbackProxyState* state = ProxiedCallbacks.Find(callbackId);
	if(!state) return false;

	if(state->Controllers.Remove(ReplicationController) <= 0) return false;

	if(state->Controllers.Num() <= 0)
	{
		ClientEventCallback(callbackId);
	}

	return true;
}

void AFunKEventBusReplicationController::ClientSendMessage_Implementation(const FFunKEventBusMessage& message)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReceiveMessage(message);

		if(message.CallbackId.IsValid())
			ServerEventCallback(message.CallbackId);
	}
}

void AFunKEventBusReplicationController::ServerSendMessage_Implementation(const FFunKEventBusMessage& message)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		TArray<AFunKEventBusReplicationController*> ReplicationControllers;
		Subsystem->GetReplicationControllers(ReplicationControllers);

		FFunKCallbackProxyState* proxyState = nullptr;
		if(message.CallbackId.IsValid())
		{
			proxyState = &ProxiedCallbacks.Emplace(message.CallbackId);
		}
		
		for (AFunKEventBusReplicationController* ReplicationController : ReplicationControllers)
		{
			if(ReplicationController != this)
			{
				if(proxyState)
					proxyState->Controllers.Add(ReplicationController);
				
				ReplicationController->ClientSendMessage(message);
			}
		}
		
		Subsystem->ReceiveMessage(message);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus/FunKEventBusReplicationController.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Net/UnrealNetwork.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "GameFramework/PlayerController.h"


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
	if (IsControllerLocallyReady() && !IsControllerReadinessSend)
	{
		IsControllerReadinessSend = true;
		ServerControllerReady();
	}
}

// Called when the game starts or when spawned
void AFunKEventBusReplicationController::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_Client)
	{
		CheckControllerReady();
	}
}

void AFunKEventBusReplicationController::ServerEventCallback_Implementation(FGuid CallbackId)
{
	if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		TArray<AFunKEventBusReplicationController*> ReplicationControllers;
		Subsystem->GetReplicationControllers(ReplicationControllers);

		for (AFunKEventBusReplicationController* ReplicationController : ReplicationControllers)
		{
			if (ReplicationController->ReportProxiedCallbackResponse(this, CallbackId))
				return;
		}

		Subsystem->CheckCallback(CallbackId, this);
	}
}

void AFunKEventBusReplicationController::ClientEventCallback_Implementation(FGuid CallbackId)
{
	if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->CheckCallback(CallbackId, this);
	}
}

void AFunKEventBusReplicationController::ServerControllerReady_Implementation()
{
	if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReplicationControllerReady(this);
	}
}

void AFunKEventBusReplicationController::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (GetNetMode() == NM_Client && GetOwner() == GetWorld()->GetFirstPlayerController())
	{
		if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
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

bool AFunKEventBusReplicationController::ReportProxiedCallbackResponse(AFunKEventBusReplicationController* ReplicationController, const FGuid CallbackId)
{
	FFunKCallbackProxyState* State = ProxiedCallbacks.Find(CallbackId);
	if (!State)
		return false;

	if (State->Controllers.Remove(ReplicationController) <= 0)
		return false;

	if (State->Controllers.Num() <= 0)
		ClientEventCallback(CallbackId);

	return true;
}

void AFunKEventBusReplicationController::ClientSendMessage_Implementation(const FFunKEventBusMessage& Message)
{
	if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReceiveMessage(Message);

		if (Message.CallbackId.IsValid())
			ServerEventCallback(Message.CallbackId);
	}
}

void AFunKEventBusReplicationController::ServerSendMessage_Implementation(const FFunKEventBusMessage& Message)
{
	if (UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		TArray<AFunKEventBusReplicationController*> ReplicationControllers;
		Subsystem->GetReplicationControllers(ReplicationControllers);

		FFunKCallbackProxyState* ProxyState = nullptr;
		if (Message.CallbackId.IsValid())
		{
			ProxyState = &ProxiedCallbacks.Emplace(Message.CallbackId);
		}
		
		for (AFunKEventBusReplicationController* ReplicationController : ReplicationControllers)
		{
			if (ReplicationController != this)
			{
				if (ProxyState)
					ProxyState->Controllers.Add(ReplicationController);
				
				ReplicationController->ClientSendMessage(Message);
			}
		}
		
		Subsystem->ReceiveMessage(Message);
	}
}

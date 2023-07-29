// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus/FunKEventBusReplicationController.h"

#include "MessageEndpoint.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AFunKEventBusReplicationController::AFunKEventBusReplicationController()
{
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

void AFunKEventBusReplicationController::ClientSendMessage_Implementation(const FFunKEventBusMessage& message)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReceiveMessage(message);
	}
}

void AFunKEventBusReplicationController::ServerSendMessage_Implementation(const FFunKEventBusMessage& message)
{
	if(UFunKEventBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UFunKEventBusSubsystem>())
	{
		Subsystem->ReceiveMessage(message);
	}
}

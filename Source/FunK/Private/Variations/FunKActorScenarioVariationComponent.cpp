// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations/FunKActorScenarioVariationComponent.h"

#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Util/FunKUtilBlueprintFunctionLibrary.h"

//TODO: Refactor this mess

// Sets default values for this component's properties
UFunKActorScenarioVariationComponent::UFunKActorScenarioVariationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}

int32 UFunKActorScenarioVariationComponent::GetCount()
{
	const EFunKTestEnvironmentType Environment = GetEnvironment();
	if(Environment == EFunKTestEnvironmentType::Standalone || Environment == EFunKTestEnvironmentType::DedicatedServer) return 1;

	int32 Count = 1;
	const FFunKOwnershipDistribution Distribution = GetOwnershipDistribution();

	if (Distribution.AppositionPlayerCount > 0) Count++;
	if (Distribution.OppositionPlayerCount > 0) Count++;

	if(Distribution.AppositionPlayerCount <= 0 && Distribution.OppositionPlayerCount > 0)
	{
		UE_LOG(FunKLog, Warning, TEXT("OppositionPlayer without AppositionPlayer are not supported!"));
	}
	
	return Count;
}

void UFunKActorScenarioVariationComponent::Begin(int32 index)
{
	const EFunKTestEnvironmentType Environment = GetEnvironment();
	if(Environment == EFunKTestEnvironmentType::Standalone || Environment == EFunKTestEnvironmentType::DedicatedServer)
	{
		Mode = EFunKActorScenarioMode::ClientToClient;
	}
	else
	{
		Mode = index == 0 ? EFunKActorScenarioMode::ClientToServer : index == 1 ? EFunKActorScenarioMode::ServerToClient : EFunKActorScenarioMode::ClientToClient;
	}
	
	if (GetNetMode() == NM_Client) return;
	
	for (const FFunKActorScenarioVariationActor& Actor : Actors)
	{
		AActor* AcquiredActor = AcquireActor(Actor);
		if (!AcquiredActor)
		{
			UE_LOG(FunKLog, Error, TEXT("Actor could not be acquired"))
		}
		else
		{
			AssignOwner(AcquiredActor, Actor.Ownership);
		}

		AcquiredActors.Add(AcquiredActor);
	}
}

bool UFunKActorScenarioVariationComponent::IsReady()
{
	if(Actors.Num() != AcquiredActors.Num()) return false;

	const ENetMode NetMode = GetNetMode();
	const EFunKClient client = UFunKBlueprintFunctionLibrary::GetClients(GetOwner());
	for (int i = 0; i < Actors.Num(); ++i)
	{
		if (!AcquiredActors[i]) return false;
		if (!AcquiredActors[i]->HasActorBegunPlay()) return false;
		if (Actors[i].Ownership != EFunKActorScenarioVariationOwnership::None)
		{
			if(NetMode < NM_Client)
			{
				if(AcquiredActors[i]->GetOwner() == nullptr) return false;
			}
			else
			{
				if (Mode == EFunKActorScenarioMode::ClientToClient)
				{
					if (client == EFunKClient::First && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
						return false;
					
					if (client == EFunKClient::Second && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
						return false;
				}
				else if (Mode == EFunKActorScenarioMode::ClientToServer)
				{
					if (client == EFunKClient::First && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
						return false;					
				}
				else if (Mode == EFunKActorScenarioMode::ServerToClient)
				{
					if (client == EFunKClient::Second && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
						return false;				
				}
			}
		}
	}

	return true;
}

void UFunKActorScenarioVariationComponent::Finish()
{
	auto NetMode = GetNetMode();
	if (NetMode < NM_Client)
	{
		for (int i = 0; i < Actors.Num(); ++i)
		{
			ReleaseActor(AcquiredActors[i], Actors[i]);
		}
	}
	
	AcquiredActors.Empty(AcquiredActors.Num());
}

FString UFunKActorScenarioVariationComponent::GetName()
{
	auto NetMode = GetNetMode();
	if(NetMode == NM_Standalone)
	{
		return "Standalone";
	}

	return FString(NetMode == NM_ListenServer ? "ListenServer " : "DedicatedServer ") + FString(Mode == EFunKActorScenarioMode::ClientToClient ? "Client->Client" : Mode == EFunKActorScenarioMode::ClientToServer ? "Client->Server" : "Server->Client");
}

void UFunKActorScenarioVariationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( UFunKActorScenarioVariationComponent, AcquiredActors );
}

AActor* UFunKActorScenarioVariationComponent::AcquireActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	if(VariationActor.SceneActor)
	{
		return GetSceneActor(VariationActor);
	}
	else
	{
		return SpawnActor(VariationActor);
	}
}

AActor* UFunKActorScenarioVariationComponent::GetSceneActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	return VariationActor.SceneActor;
}

AActor* UFunKActorScenarioVariationComponent::SpawnActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	return GetWorld()->SpawnActor(VariationActor.SpawnActor.Class);
}

void UFunKActorScenarioVariationComponent::AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership)
{
	auto Controller = GetController(Ownership);

	if(APawn* Pawn = Cast<APawn>(Actor))
	{
		// Pawn->PossessedBy(PlayerController);
		Controller->Possess(Pawn);
	}
	else
	{
		Actor->SetOwner(Controller);
		Actor->SetAutonomousProxy(true);
		Actor->ForceNetUpdate();
	}
	
	Actor->ForceNetUpdate();
}

void UFunKActorScenarioVariationComponent::ReleaseActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor)
{
	if (!Actor) return;

	if(VariationActor.Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer || VariationActor.Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer)
	{
		APlayerController* Controller = GetController(VariationActor.Ownership);
		if(Controller && Controller->GetPawn() == Actor)
		{
			Controller->UnPossess();
		}
	}
	
	if(VariationActor.SceneActor)
	{
		return ReleaseSceneActor(Actor, VariationActor);
	}
	else
	{
		return ReleaseSpawnActor(Actor, VariationActor);
	}
}

void UFunKActorScenarioVariationComponent::ReleaseSceneActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor)
{
	Actor->Reset();
}

void UFunKActorScenarioVariationComponent::ReleaseSpawnActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor)
{
	GetWorld()->RemoveActor(Actor, true);
}

APlayerController* UFunKActorScenarioVariationComponent::GetController(EFunKActorScenarioVariationOwnership Ownership)
{
	if (Ownership == EFunKActorScenarioVariationOwnership::None) return nullptr;
	if (Ownership == EFunKActorScenarioVariationOwnership::AI)
	{
		return nullptr;
	}

	auto NetMode = GetNetMode();
	auto It = GetWorld()->GetPlayerControllerIterator();

	switch (Mode)
	{
	case EFunKActorScenarioMode::ClientToClient:
		if (NetMode == NM_ListenServer) ++It; // Skip playable Server
		if (Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) ++It; // Skip AppositionPlayer
		return It->Get(); 
	case EFunKActorScenarioMode::ClientToServer:
		if(Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) return It->Get(); // Take playable Server
		++It; // Skip playable Server
		return It->Get(); 
	case EFunKActorScenarioMode::ServerToClient:
		if(Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer) return It->Get(); // Take playable Server
		++It; // Skip playable Server
		return It->Get(); 
	default: ;
	}
	
	check(false);
	return nullptr;
}

EFunKTestEnvironmentType UFunKActorScenarioVariationComponent::GetEnvironment() const
{
	const ENetMode NetMode = GetNetMode();
	if(NetMode == NM_Standalone) return EFunKTestEnvironmentType::Standalone;
	if(NetMode == NM_Standalone) return EFunKTestEnvironmentType::ListenServer;
	if(GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->IsServerDedicated()) return EFunKTestEnvironmentType::DedicatedServer;
	return EFunKTestEnvironmentType::ListenServer;
}

FFunKOwnershipDistribution UFunKActorScenarioVariationComponent::GetOwnershipDistribution()
{
	FFunKOwnershipDistribution OwnershipDistribution(0, 0, 0, 0);
	
	for (const FFunKActorScenarioVariationActor& Actor : Actors)
	{
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::None) OwnershipDistribution.NoneCount++;
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer) OwnershipDistribution.AppositionPlayerCount++;
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) OwnershipDistribution.OppositionPlayerCount++;
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::AI) OwnershipDistribution.AICount++;
	}

	return OwnershipDistribution;
}

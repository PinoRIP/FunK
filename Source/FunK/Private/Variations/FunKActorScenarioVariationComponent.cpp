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
	auto NetMode = GetNetMode();
	int32 Counter = 1;
	if (NetMode == NM_Standalone) return Counter;

	bool hasAppositionPlayer = false, hasOppositionPlayer = false;
	for (FFunKActorScenarioVariationActor& Actor : Actors)
	{
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer) hasAppositionPlayer = true;
		if(Actor.Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) hasOppositionPlayer = true;
		if(hasAppositionPlayer && hasOppositionPlayer) break;
	}

	if (!hasAppositionPlayer && !hasOppositionPlayer) return Counter;
	if(GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->IsServerDedicated())
	{
		if (hasAppositionPlayer && hasOppositionPlayer) Counter++;
	}
	else
	{
		if(hasAppositionPlayer) Counter++;
		if(hasOppositionPlayer) Counter++;
	}

	if(!hasAppositionPlayer && hasOppositionPlayer)
	{
		UE_LOG(FunKLog, Warning, TEXT("OppositionPlayer without AppositionPlayer are not supported!"));
	}

	return Counter;
}

void UFunKActorScenarioVariationComponent::Begin(int32 index)
{
	auto NetMode = GetNetMode();
	if(NetMode == NM_Standalone)
	{
		Mode = EFunKActorScenarioMode::Standalone;
	}
	else if(GetWorld()->GetSubsystem<UFunKWorldSubsystem>()->IsServerDedicated())
	{
		Mode = index == 0 ? EFunKActorScenarioMode::ClientToServer : EFunKActorScenarioMode::ClientToClient;
	}
	else
	{
		Mode = index == 0 ? EFunKActorScenarioMode::ClientToServer : index == 1 ? EFunKActorScenarioMode::ServerToClient : EFunKActorScenarioMode::ClientToClient;
	}
	
	if (NetMode == NM_Client) return;
	
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
					if (client == EFunKClient::Second && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
						return false;

					if (client == EFunKClient::First && Actors[i].Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer && AcquiredActors[i]->GetOwner() == nullptr)
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

	if(Mode == EFunKActorScenarioMode::ClientToClient || Mode == EFunKActorScenarioMode::Standalone)
	{
		if (NetMode == NM_ListenServer) ++It; // Skip playable Server
		if (Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) ++It; // Skip AppositionPlayer
		return It->Get(); 
	}
	else if(Mode == EFunKActorScenarioMode::ClientToServer)
	{
		if(Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) return It->Get(); // Take playable Server
		else ++It; // Skip playable Server

		return It->Get(); 
	}
	else if(Mode == EFunKActorScenarioMode::ServerToClient)
	{
		if(Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer) return It->Get(); // Take playable Server
		else ++It; // Skip playable Server

		return It->Get(); 
	}

	check(false);
	return nullptr;
}

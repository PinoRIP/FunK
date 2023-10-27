// Fill out your copyright notice in the Description page of Project Settings.


#include "Extensions//FunKActorScenarioVariationComponent.h"
#include "FunKLogging.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Util/FunKUtilBlueprintFunctionLibrary.h"
#include "Extensions/FunKSceneActorResetHandler.h"


void UFunKActorScenarioVariationFragment::OnAdded()
{
	Spawner->Begin(Index);
}

void UFunKActorScenarioVariationFragment::OnRemoved()
{
	Spawner->Finish();
}

FString UFunKActorScenarioVariationFragment::GetReadableIdent() const
{
	return Spawner->GetName();
}

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

UFunKTestFragment* UFunKActorScenarioVariationComponent::GetFragment(int32 Index)
{
	UFunKActorScenarioVariationFragment* Fragment = NewObject<UFunKActorScenarioVariationFragment>(this);
	Fragment->Spawner = this;
	Fragment->Index = Index;
	return Fragment;
}

bool UFunKActorScenarioVariationComponent::IsReady(UFunKTestFragment* Instance, int32 Index)
{
	return Instance != nullptr && Cast<UFunKActorScenarioVariationFragment>(Instance) && IsReady();
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

	auto NetMode = GetNetMode();
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
			if(NetMode != NM_Standalone && !AcquiredActor->GetIsReplicated())
			{
				UE_LOG(FunKLog, Warning, TEXT("Actor %s is not replicated!"), *AcquiredActor->GetName())
			}
			
			AssignOwner(AcquiredActor, Actor.Ownership);
		}

		AcquiredActors.Add(AcquiredActor);
	}
}

bool UFunKActorScenarioVariationComponent::IsReady()
{
	if(Actors.Num() != AcquiredActors.Num()) return false;

	const ENetMode NetMode = GetNetMode();
	const uint8 LocalOwnerships = static_cast<uint8>(GetLocalOwnerships());
	if(LocalOwnerships == 0) return true;
	
	for (int i = 0; i < Actors.Num(); ++i)
	{
		if (!AcquiredActors[i]) return false;
		if (!AcquiredActors[i]->HasActorBegunPlay()) return false;
		if (Actors[i].Ownership != EFunKActorScenarioVariationOwnership::None)
		{
			if(NetMode < NM_Client || (LocalOwnerships & static_cast<uint8>(Actors[i].Ownership)) == LocalOwnerships)
			{
				if(AcquiredActors[i]->GetOwner() == nullptr) return false;
			}
		}
	}

	return true;
}

void UFunKActorScenarioVariationComponent::Finish()
{
	const ENetMode NetMode = GetNetMode();
	if (NetMode < NM_Client)
	{
		for (int i = 0; i < Actors.Num(); ++i)
		{
			ReleaseActor(AcquiredActors[i], Actors[i]);
		}
	}
	
	AcquiredActors.Empty(AcquiredActors.Num());
}

FString UFunKActorScenarioVariationComponent::GetName() const
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

AActor* UFunKActorScenarioVariationComponent::GetActor(int32 InIndex)
{
	if(InIndex < 0 || InIndex >= AcquiredActors.Num()) return nullptr;
	return AcquiredActors[InIndex];
}

AActor* UFunKActorScenarioVariationComponent::GetActorByOwnership(EFunKActorScenarioVariationOwnership Ownership, int32 InIndex)
{
	if(InIndex < 0 || InIndex >= AcquiredActors.Num()) return nullptr;
	int32 OwnershipIndex = 0;

	for (int i = 0; i < Actors.Num(); ++i)
	{
		if(Actors[i].Ownership == Ownership)
		{
			if(OwnershipIndex == InIndex) return AcquiredActors[i];
			OwnershipIndex++;
		}
	}

	return nullptr;
}

AActor* UFunKActorScenarioVariationComponent::AcquireActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	if(VariationActor.SceneActor.Actor)
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
	//FActorSpawnParameters ActorSpawnParameters = FActorSpawnParameters();
	//ActorSpawnParameters.Template = VariationActor.SceneActor;
	//ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//ActorSpawnParameters.bDeferConstruction = true;

	//AActor* Actor = GetWorld()->SpawnActor(VariationActor.SceneActor->GetClass(), &VariationActor.SceneActor->GetTransform(), FActorSpawnParameters(ActorSpawnParameters));
	//Actor->SetActorEnableCollision(false);

	//Actor->FinishSpawning(VariationActor.SceneActor->GetTransform());

	UFunKSceneActorResetHandler* Handler = NewObject<UFunKSceneActorResetHandler>(this, VariationActor.SceneActor.ResetHandlerClass);
	Handler->Capture(VariationActor.SceneActor.Actor);

	ResetHandlers.Add(VariationActor.SceneActor.Actor, Handler);
	
	//InitialActorStates.Add(Actor);
	return VariationActor.SceneActor.Actor;
}

AActor* UFunKActorScenarioVariationComponent::SpawnActor(const FFunKActorScenarioVariationActor& VariationActor)
{
	return GetWorld()->SpawnActor(VariationActor.SpawnActor.Class, &VariationActor.SpawnActor.Transform);
}

void UFunKActorScenarioVariationComponent::AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership)
{
	APlayerController* Controller = GetController(Ownership);
	if(!Controller) return;

	const ENetMode NetMode = GetNetMode();
	if(APawn* Pawn = Cast<APawn>(Actor))
	{
		// Pawn->PossessedBy(PlayerController);
		Controller->Possess(Pawn);
	}
	else
	{
		Actor->SetOwner(Controller);

		if(NetMode != NM_Standalone)
			Actor->SetAutonomousProxy(true);
	}

	if(NetMode != NM_Standalone)
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
			Actor->ForceNetUpdate();
		}
	}
	
	if(VariationActor.SceneActor.Actor)
	{
		ReleaseSceneActor(Actor, VariationActor);
	}
	else
	{
		ReleaseSpawnActor(Actor, VariationActor);
	}
}

void UFunKActorScenarioVariationComponent::ReleaseSceneActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor)
{
	//Actor->GetClass()->Properties

	//UGameplayStatics::OpenLevel()
	//
	//AActor* InitialActorState = InitialActorStates[0];
	//
	//CopyPropertiesFromTo(InitialActorState, Actor);
	//Actor->SetActorEnableCollision(true);

	//InitialActorStates.RemoveAt(0);
	//GetWorld()->DestroyActor(InitialActorState, true, true);
	
	// Actor->Reset();

	UFunKSceneActorResetHandler* Handler = ResetHandlers[Actor];
	Handler->Reset(Actor);

	ResetHandlers.Remove(Actor);
}

//void UFunKActorScenarioVariationComponent::CopyPropertiesFromTo(UObject* From, UObject* To)
//{
//	for( TFieldIterator<FProperty> Prop(From->GetClass()); Prop; ++Prop ) {
//		// Check if the property is not marked as transient
//		if (!Prop->HasAnyPropertyFlags(CPF_Transient))
//		{
//			// Get the property's value from the source actor
//			void* SourceValue = Prop->ContainerPtrToValuePtr<void>(From);
//
//			// Set the property's value on the target actor
//			Prop->CopyCompleteValue(Prop->ContainerPtrToValuePtr<void>(To), SourceValue);
//		}
//	}
//}

void UFunKActorScenarioVariationComponent::ReleaseSpawnActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor)
{
	GetWorld()->DestroyActor(Actor, true, true);
}

#define FUNK_NEXT_CONTROLLER(It) if(!++It) { UE_LOG(FunKLog, Error, TEXT("Invalid controller access")) return nullptr; }

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
		if (NetMode == NM_ListenServer) FUNK_NEXT_CONTROLLER(It); // Skip playable Server
		if (Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) FUNK_NEXT_CONTROLLER(It); // Skip AppositionPlayer
		return It->Get(); 
	case EFunKActorScenarioMode::ClientToServer:
		if(Ownership == EFunKActorScenarioVariationOwnership::OppositionPlayer) return It->Get(); // Take playable Server
		FUNK_NEXT_CONTROLLER(It); // Skip playable Server
		return It->Get(); 
	case EFunKActorScenarioMode::ServerToClient:
		if(Ownership == EFunKActorScenarioVariationOwnership::AppositionPlayer) return It->Get(); // Take playable Server
		FUNK_NEXT_CONTROLLER(It); // Skip playable Server
		FUNK_NEXT_CONTROLLER(It); // Skip client1
		return It->Get(); 
	default: ;
	}
	
	check(false);
	return nullptr;
}

EFunKTestEnvironmentType UFunKActorScenarioVariationComponent::GetEnvironment() const
{
	return UFunKBlueprintFunctionLibrary::GetTestEnvironmentType(this);
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

EFunKActorScenarioVariationOwnership UFunKActorScenarioVariationComponent::GetLocalOwnerships()
{
	const ENetMode NetMode = GetNetMode();
	if(NetMode == NM_Standalone) return EFunKActorScenarioVariationOwnership::AppositionPlayer | EFunKActorScenarioVariationOwnership::OppositionPlayer | EFunKActorScenarioVariationOwnership::AI;
	if(NetMode == NM_DedicatedServer) return EFunKActorScenarioVariationOwnership::AI;
	if(NetMode == NM_ListenServer)
	{
		constexpr EFunKActorScenarioVariationOwnership Default = EFunKActorScenarioVariationOwnership::AI;
		if (Mode == EFunKActorScenarioMode::ServerToClient) return Default | EFunKActorScenarioVariationOwnership::AppositionPlayer;
		if (Mode == EFunKActorScenarioMode::ClientToServer) return Default | EFunKActorScenarioVariationOwnership::OppositionPlayer;
		return Default;
	}

	const EFunKClient client = UFunKBlueprintFunctionLibrary::GetClients(GetOwner());
	switch (Mode)
	{
	case EFunKActorScenarioMode::ClientToClient:
		return client == EFunKClient::First ? EFunKActorScenarioVariationOwnership::AppositionPlayer : EFunKActorScenarioVariationOwnership::OppositionPlayer;
	case EFunKActorScenarioMode::ClientToServer:
		return client == EFunKClient::First ? EFunKActorScenarioVariationOwnership::AppositionPlayer : EFunKActorScenarioVariationOwnership::None;
	case EFunKActorScenarioMode::ServerToClient:
		return client == EFunKClient::First ? EFunKActorScenarioVariationOwnership::None : EFunKActorScenarioVariationOwnership::OppositionPlayer;
	default: return EFunKActorScenarioVariationOwnership::None;
	}
}

bool UFunKActorScenarioVariationComponent::IsAppositionPlayer()
{
	const uint8 LocalOwnerships = static_cast<uint8>(GetLocalOwnerships());
	if (LocalOwnerships == 0) return false;
	return (LocalOwnerships & static_cast<uint8>(EFunKActorScenarioVariationOwnership::AppositionPlayer)) == LocalOwnerships;
}

bool UFunKActorScenarioVariationComponent::IsOppositionPlayer()
{
	const uint8 LocalOwnerships = static_cast<uint8>(GetLocalOwnerships());
	if (LocalOwnerships == 0) return false;
	return (LocalOwnerships & static_cast<uint8>(EFunKActorScenarioVariationOwnership::OppositionPlayer)) == LocalOwnerships;
}

bool UFunKActorScenarioVariationComponent::IsAI()
{
	const uint8 LocalOwnerships = static_cast<uint8>(GetLocalOwnerships());
	if (LocalOwnerships == 0) return false;
	return (LocalOwnerships & static_cast<uint8>(EFunKActorScenarioVariationOwnership::AI)) == LocalOwnerships;
}

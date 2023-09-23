// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenario/FunKActorScenarioSpawnOwnedActorComponent.h"
#include "GameFramework/PlayerController.h"
#include "Util\FunKUtilBlueprintFunctionLibrary.h"


// Sets default values for this component's properties
UFunKActorScenarioSpawnOwnedActorComponent::UFunKActorScenarioSpawnOwnedActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

AActor* UFunKActorScenarioSpawnOwnedActorComponent::AcquireActor(const FFunKActorScenario& Scenario)
{
	AActor* actor = Super::AcquireActor(Scenario);
	AssignOwner(actor, Scenario);
	return actor;
}

void UFunKActorScenarioSpawnOwnedActorComponent::AssignOwner(AActor* Actor, const FFunKActorScenario& Scenario)
{
	auto NetMode = GetNetMode();

	// No owners in standalone
	if(NetMode != NM_Standalone)
	{
		check(Actor->GetIsReplicated());
	}
	
	AssignOwner(Actor, GetController(Scenario));
}

void UFunKActorScenarioSpawnOwnedActorComponent::AssignOwner(AActor* Actor, APlayerController* PlayerController)
{
	if(!Actor || !PlayerController) return;

	if(APawn* Pawn = Cast<APawn>(Actor))
	{
		// Pawn->PossessedBy(PlayerController);
		PlayerController->Possess(Pawn);
	}
	else
	{
		Actor->SetOwner(PlayerController);
		Actor->SetAutonomousProxy(true);
		Actor->ForceNetUpdate();
	}
	
	Actor->ForceNetUpdate();
}

bool UFunKActorScenarioSpawnOwnedActorComponent::VerifyActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
	if(!Super::VerifyActor(Actor, Scenario))
		return false;

	const EFunKTestLocationTarget ActorLocationTarget = IsOppositionActor
		? Scenario.GetTo()
		: Scenario.GetFrom();

	if(ActorLocationTarget == EFunKTestLocationTarget::Standalone || ActorLocationTarget == EFunKTestLocationTarget::DedicatedServer || ActorLocationTarget == EFunKTestLocationTarget::ListenServer)
		return true;

	// Dont check the owner on the other client -> The owner will only be replicated to the owning client.
	const ENetMode NetMode = GetNetMode();
	if(NetMode == NM_Client)
	{
		const EFunKClient client = UFunKBlueprintFunctionLibrary::GetClients(GetOwner());
		if(client == EFunKClient::Second && !IsOppositionActor)
			return true;

		if(client == EFunKClient::First && IsOppositionActor)
			return true;
	}

	return Actor->GetOwner() != nullptr;
}

void UFunKActorScenarioSpawnOwnedActorComponent::ReleaseActor(AActor* Actor, const FFunKActorScenario& Scenario)
{
	APlayerController* Controller = GetController(Scenario);

	if(Controller && Controller->GetPawn() == Actor)
	{
		Controller->UnPossess();
	}
	
	Super::ReleaseActor(Actor, Scenario);
}

APlayerController* UFunKActorScenarioSpawnOwnedActorComponent::GetController(const FFunKActorScenario& Scenario) const
{
	const EFunKTestLocationTarget ActorLocationTarget = IsOppositionActor
		? Scenario.GetTo()
		: Scenario.GetFrom();

	// No explicit owner if the dedicated server should take ownership
	if(ActorLocationTarget == EFunKTestLocationTarget::DedicatedServer)
		return nullptr;

	auto It = GetWorld()->GetPlayerControllerIterator();
	if(ActorLocationTarget == EFunKTestLocationTarget::ListenServer)
	{
		return It->Get();
	}

	const ENetMode NetMode = GetNetMode();
	// Only client to client scenarios should be left now. Skip the listen sever controller so that the iterator behaves the same for dedicated & listen servers.
	if(NetMode == NM_ListenServer)
		++It;

	// The opposition in client to client is client 2
	if(IsOppositionActor)
		++It;

	return It->Get();
}

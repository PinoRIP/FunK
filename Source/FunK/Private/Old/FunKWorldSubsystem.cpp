// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/FunKWorldSubsystem.h"

#include "EngineUtils.h"
#include "Old/FunKLogging.h"
#include "Old/FunKWorldTestController.h"
#include "Old/FunKSettingsObject.h"
#include "Old/FunKTestBase.h"
#include "Old/EventBus/FunKEventBusReplicationController.h"
#include "Old/EventBus/FunKEventBusSubsystem.h"
#include "Old/Extensions/FunKTestVariationsWorldActor.h"
#include "Old/Extensions/FunKTestRootVariationComponent.h"
#include "Old/Extensions/FunKTestVariationComponent.h"

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if (!LocalTestController)
	{
		for (TActorIterator<AFunKWorldTestController> It(GetWorld(), AFunKWorldTestController::StaticClass()); It; ++It)
		{
			if (LocalTestController)
			{
				UE_LOG(FunKLog, Error, TEXT("Only one AFunKWorldTestController per world allowed!"))
				break;
			}

			LocalTestController = *It;
		}

		if (!LocalTestController)
			LocalTestController = NewTestController();
	}
	
	return LocalTestController;
}

int32 UFunKWorldSubsystem::GetPeerIndex() const
{
	const UWorld* World = GetWorld();

	if (World->GetNetMode() < NM_Client)
		return 0;

	const UFunKEventBusSubsystem* FunkEventSubsystem = World->GetSubsystem<UFunKEventBusSubsystem>();
	if (!FunkEventSubsystem)
		return INDEX_NONE;

	const AFunKEventBusReplicationController* FunkEventController = FunkEventSubsystem->GetLocalController();
	if (!FunkEventController)
		return INDEX_NONE;

	return FunkEventController->GetControllerNumber();
}

int32 UFunKWorldSubsystem::GetPeerCount() const
{
	const UWorld* World = GetWorld();

	if (World->GetNetMode() == NM_Standalone)
		return 1;

	const UFunKEventBusSubsystem* FunkEventSubsystem = World->GetSubsystem<UFunKEventBusSubsystem>();
	if (!FunkEventSubsystem)
		return 1;

	return FunkEventSubsystem->GetReplicationControllerCount() + 1;
}

bool UFunKWorldSubsystem::IsServerDedicated() const
{
	const UWorld* World = GetWorld();
	const ENetMode NetMode = World->GetNetMode();

	if (NetMode < NM_Client)
		return NetMode == NM_DedicatedServer;

	const UFunKEventBusSubsystem* FunkEventSubsystem = World->GetSubsystem<UFunKEventBusSubsystem>();
	if (!FunkEventSubsystem)
		return false;

	const AFunKEventBusReplicationController* FunkEventController = FunkEventSubsystem->GetLocalController();
	if (!FunkEventController)
		return false;

	return FunkEventController->GetIsServerDedicated();
}

const FFunKWorldVariations& UFunKWorldSubsystem::GetWorldVariations()
{
	if (!AreVariationsGathered)
	{
		GatherVariations(Variations);
		AreVariationsGathered = true;
	}

	return Variations;
}

void UFunKWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	UFunKEventBusSubsystem* EventBusSubsystem = InWorld.GetSubsystem<UFunKEventBusSubsystem>();
	if (!EventBusSubsystem)
	{
		UE_LOG(FunKLog, Error, TEXT("Missing event bus subsystem!"))
		return;
	}

	AFunKTestBase::RegisterEvents(EventBusSubsystem);
}

AFunKWorldTestController* UFunKWorldSubsystem::NewTestController() const
{
	UClass* ReplicatedManagerClass = GetDefault<UFunKSettingsObject>()->Settings.WorldTestControllerClassOverride.Get();
	return ReplicatedManagerClass
		? GetWorld()->SpawnActor<AFunKWorldTestController>(ReplicatedManagerClass)
		: GetWorld()->SpawnActor<AFunKWorldTestController>();
}

void UFunKWorldSubsystem::GatherVariations(FFunKWorldVariations& OutVariations) const
{
	TArray<AFunKTestVariationsWorldActor*> SharedTestVariationActors;
	for (TActorIterator<AFunKTestVariationsWorldActor> ActorItr(GetWorld(), AFunKTestVariationsWorldActor::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		SharedTestVariationActors.Add(*ActorItr);
	}

	SharedTestVariationActors.Sort([](const AFunKTestVariationsWorldActor& A, const AFunKTestVariationsWorldActor& B) {
		return  A.GetName() < B.GetName();
	});

	OutVariations.Variations.Empty();
	OutVariations.VariationCount = 0;
	for (const AFunKTestVariationsWorldActor* SharedTestVariationActor : SharedTestVariationActors)
	{
		TArray<UFunKTestVariationComponent*> Array;
		SharedTestVariationActor->GetComponents<UFunKTestVariationComponent>(Array);
		
		Array.Sort([](const UFunKTestVariationComponent& A, const UFunKTestVariationComponent& B) {
			return  A.GetFName().FastLess(B.GetFName());
		});

		for (UFunKTestVariationComponent* FunKTestVariationComponent : Array)
		{
			if (FunKTestVariationComponent->IsA(UFunKTestRootVariationComponent::StaticClass()))
			{
				UE_LOG(FunKLog, Error, TEXT("Root variation components are not allowed on shared test variations! %s"), *SharedTestVariationActor->GetName())
				continue;
			}
				
			OutVariations.Variations.Add(FunKTestVariationComponent);
			OutVariations.VariationCount += FunKTestVariationComponent->GetCount();
		}
	}
}

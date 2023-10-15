// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldSubsystem.h"

#include "EngineUtils.h"
#include "FunKLogging.h"
#include "FunKWorldTestController.h"
#include "Internal/FunKSettingsObject.h"
#include "Internal/FunKTestBase.h"
#include "Internal/EventBus/FunKEventBusReplicationController.h"
#include "Internal/EventBus/FunKEventBusSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Variations/FunKSharedTestVariations.h"
#include "Variations/FunKTestRootVariationComponent.h"
#include "Variations/FunKTestVariationComponent.h"

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if(!LocalTestController)
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

		if(!LocalTestController)
			LocalTestController = NewTestController();
	}
	
	return LocalTestController;
}

int32 UFunKWorldSubsystem::GetPeerIndex() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode < NM_Client) return 0;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return INDEX_NONE;

	const AFunKEventBusReplicationController* funkEventController = funkEventSubsystem->GetLocalController();
	if(!funkEventController) return INDEX_NONE;

	return funkEventController->GetControllerNumber();
}

int32 UFunKWorldSubsystem::GetPeerCount() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();
	if(netMode == NM_Standalone) return 1;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return 1;

	return funkEventSubsystem->GetReplicationControllerCount() + 1;
}

bool UFunKWorldSubsystem::IsServerDedicated() const
{
	UWorld* world = GetWorld();
	auto netMode = world->GetNetMode();

	if(netMode < NM_Client) return netMode == NM_DedicatedServer;

	const UFunKEventBusSubsystem* funkEventSubsystem = world->GetSubsystem<UFunKEventBusSubsystem>();
	if(!funkEventSubsystem) return false;

	const AFunKEventBusReplicationController* funkEventController = funkEventSubsystem->GetLocalController();
	if(!funkEventController) return false;

	return funkEventController->GetIsServerDedicated();
}

const FFunKWorldVariations& UFunKWorldSubsystem::GetWorldVariations()
{
	if(!AreVariationsGathered)
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
	if(!EventBusSubsystem)
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
	TArray<AFunKSharedTestVariations*> SharedTestVariationActors;
	for (TActorIterator<AFunKSharedTestVariations> ActorItr(GetWorld(), AFunKSharedTestVariations::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
	{
		SharedTestVariationActors.Add(*ActorItr);
	}

	SharedTestVariationActors.Sort([](const AFunKSharedTestVariations& ip1, const AFunKSharedTestVariations& ip2) {
		return  ip1.GetName() < ip2.GetName();
	});

	OutVariations.Variations.Empty();
	OutVariations.VariationCount = 0;
	for (const AFunKSharedTestVariations* SharedTestVariationActor : SharedTestVariationActors)
	{
		TArray<UFunKTestVariationComponent*> Array;
		SharedTestVariationActor->GetComponents<UFunKTestVariationComponent>(Array);
		
		Array.Sort([](const UFunKTestVariationComponent& ip1, const UFunKTestVariationComponent& ip2) {
			return  ip1.GetFName().FastLess(ip2.GetFName());
		});

		for (UFunKTestVariationComponent* FunKTestVariationComponent : Array)
		{
			if(FunKTestVariationComponent->IsA(UFunKTestRootVariationComponent::StaticClass()))
			{
				UE_LOG(FunKLog, Error, TEXT("Root variation components are not allowed on shared test variations! %s"), *SharedTestVariationActor->GetName())
				continue;
			}
				
			OutVariations.Variations.Add(FunKTestVariationComponent);
			OutVariations.VariationCount += FunKTestVariationComponent->GetCount();
		}
	}
}

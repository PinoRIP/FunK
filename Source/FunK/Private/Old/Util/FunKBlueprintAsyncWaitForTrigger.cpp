// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Util/FunKBlueprintAsyncWaitForTrigger.h"

#include "Old/FunKLogging.h"
#include "Engine/TriggerBase.h"
#include "Old/FunKTestBase.h"

UFunKBlueprintAsyncWaitForTrigger* UFunKBlueprintAsyncWaitForTrigger::WaitFor(AFunKTestBase* Test, ATriggerBase* Trigger)
{
	if (!Test)
		return nullptr;
	
	UFunKBlueprintAsyncWaitForTrigger* WaitForTriggerBoxInstance = NewObject<UFunKBlueprintAsyncWaitForTrigger>(Test);
	WaitForTriggerBoxInstance->Trigger = Trigger;
	
	return WaitForTriggerBoxInstance;
}

void UFunKBlueprintAsyncWaitForTrigger::Activate()
{
	Super::Activate();

	if (!Trigger)
	{
		UE_LOG(FunKLog, Error, TEXT("Could not wait for trigger nullptr"))
		return;
	}

	Trigger->OnActorBeginOverlap.AddDynamic(this, &UFunKBlueprintAsyncWaitForTrigger::OnTriggeredActorBeginOverlap);
}

void UFunKBlueprintAsyncWaitForTrigger::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();
}

void UFunKBlueprintAsyncWaitForTrigger::OnTriggeredActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor)
		return;
	
	if (Trigger)
	{
		UE_LOG(FunKLog, Log, TEXT("%s triggered %s."), *OtherActor->GetName(), *Trigger->GetName());
	}

	OnTriggered.Broadcast(OtherActor);
}

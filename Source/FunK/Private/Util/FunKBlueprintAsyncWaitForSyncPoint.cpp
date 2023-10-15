// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKBlueprintAsyncWaitForSyncPoint.h"
#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "FunKTestBase.h"
#include "Events/FunKTestWaitForSyncPointReachedEvent.h"

UFunKBlueprintAsyncWaitForSyncPoint* UFunKBlueprintAsyncWaitForSyncPoint::WaitFor(AFunKTestBase* Test, EFunKSyncTarget WaitFor)
{
	if(!Test || WaitFor == EFunKSyncTarget::MAX)
		return nullptr;

	UFunKBlueprintAsyncWaitForSyncPoint* SyncPointInstance = NewObject<UFunKBlueprintAsyncWaitForSyncPoint>(Test);
	SyncPointInstance->Test = Test;
	SyncPointInstance->SyncTarget = WaitFor;
	SyncPointInstance->TargetStageIndex = Test->GetCurrentStageIndex();

	const ENetMode NetMode = Test->GetNetMode();
	if(NetMode != NM_Standalone)
	{
		SyncPointInstance->Bitmask = FFunKAnonymousBitmask(SyncPointInstance->Test->GetWorldSubsystem()->GetPeerCount());
		SyncPointInstance->Bitmask.Set(SyncPointInstance->Test->GetStagePeerState());
		if(WaitFor == EFunKSyncTarget::Client)
		{
			SyncPointInstance->Bitmask.Set(0);
		}
		else if(WaitFor == EFunKSyncTarget::Server)
		{
			SyncPointInstance->Bitmask.SetAll();
			SyncPointInstance->Bitmask.Clear(0);
		}
		
		SyncPointInstance->Test->GetEventBusSubsystem()->On<FFunKTestWaitForSyncPointReachedEvent>([SyncPointInstance](const FFunKTestWaitForSyncPointReachedEvent& Event)
		{
			if(Event.Test == SyncPointInstance->Test)
			{
				SyncPointInstance->Received(Event.PeerIndex);
			}
		});

		SyncPointInstance->Test->OnTestFinish.AddDynamic(SyncPointInstance, &UFunKBlueprintAsyncWaitForSyncPoint::Disable);
		SyncPointInstance->Test->OnPeerStageFinishing.AddDynamic(SyncPointInstance, &UFunKBlueprintAsyncWaitForSyncPoint::StageFinish);
	}

	return SyncPointInstance;
}

void UFunKBlueprintAsyncWaitForSyncPoint::Activate()
{
	Super::Activate();

	if(!Test)
	{
		UE_LOG(FunKLog, Error, TEXT("Target Test is null in WaitFor"))
		return;
	}

	const ENetMode NetMode = Test->GetNetMode();
	if(NetMode == NM_Standalone)
	{
		Broadcast();
		return;
	}

	FFunKTestWaitForSyncPointReachedEvent SyncPointReachedEvent;
	SyncPointReachedEvent.Test = Test;
	SyncPointReachedEvent.PeerIndex = Test->GetWorldSubsystem()->GetPeerIndex();
	
	Test->GetEventBusSubsystem()->Raise(SyncPointReachedEvent);
}

void UFunKBlueprintAsyncWaitForSyncPoint::SetReadyToDestroy()
{
	Disable();
	if(Test)
	{
		Test->OnTestFinish.RemoveDynamic(this, &UFunKBlueprintAsyncWaitForSyncPoint::Disable);
		Test->OnPeerStageFinishing.RemoveDynamic(this, &UFunKBlueprintAsyncWaitForSyncPoint::StageFinish);
	}
	Test = nullptr;
	
	Super::SetReadyToDestroy();
}

void UFunKBlueprintAsyncWaitForSyncPoint::Broadcast()
{
	if(!IsBroadcastExecuted)
	{
		IsBroadcastExecuted = true;
		OnSyncPointReady.Broadcast();
	}
	
	Disable();
}

void UFunKBlueprintAsyncWaitForSyncPoint::StageFinish(int32 PeerIndex)
{
	Received(PeerIndex);
}

void UFunKBlueprintAsyncWaitForSyncPoint::Received(int32 PeerIndex)
{
	if(!Test) return;
	
	if(TargetStageIndex != Test->GetCurrentStageIndex())
	{
		Disable();
		return;
	}
	
	Bitmask.Set(PeerIndex);

	if(Bitmask.IsSet())
		Broadcast();
}

void UFunKBlueprintAsyncWaitForSyncPoint::Disable()
{
	IsBroadcastExecuted = true;
	Registration.Unregister();
}

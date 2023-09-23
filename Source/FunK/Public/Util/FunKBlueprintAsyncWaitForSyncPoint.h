// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Internal/Util/FunKAnonymousBitmask.h"
#include "Internal/EventBus/FunKEventBusSubsystem.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FunKBlueprintAsyncWaitForSyncPoint.generated.h"

class AFunKTestBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFunKSyncPointReady);

UENUM(BlueprintType)
enum class EFunKSyncTarget : uint8
{
	Server,
	Client,
	All,

	MAX UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKBlueprintAsyncWaitForSyncPoint : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FFunKSyncPointReady OnSyncPointReady;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "Test", DefaultToSelf = "Test", BlueprintInternalUseOnly = "true"), Category = "AsyncNode", DisplayName="WaitFor (Sync point)")
	static UFunKBlueprintAsyncWaitForSyncPoint* WaitFor(AFunKTestBase* Test, EFunKSyncTarget WaitFor);

	virtual void Activate() override;

	virtual void SetReadyToDestroy() override;

private:
	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	EFunKSyncTarget SyncTarget = EFunKSyncTarget::MAX;

	FFunKAnonymousBitmask Bitmask;

	FFunKEventBusRegistration Registration;

	bool IsBroadcastExecuted = false;

	int32 TargetStageIndex = INDEX_NONE;
	
	UFUNCTION()
	void StageFinish(int32 PeerIndex);
	
	UFUNCTION()
	void Disable();
	
	void Broadcast();
	void Received(int32 PeerIndex);
};

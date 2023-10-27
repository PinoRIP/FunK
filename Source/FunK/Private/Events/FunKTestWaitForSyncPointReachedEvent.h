// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestWaitForSyncPointReachedEvent.generated.h"

class AFunKTestBase;

// Event that is used to sync peers at a "wait for" sync-point
USTRUCT()
struct FFunKTestWaitForSyncPointReachedEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY()
	int32 PeerIndex = 0;
};


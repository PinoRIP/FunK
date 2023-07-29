// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/FunKTestBase.h"
#include "FunKTestWaitForSyncPointReachedEvent.generated.h"

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


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Setup/FunKTimeLimit.h"
#include "FunKStage.generated.h"

DECLARE_DELEGATE(FFunKStageFunction);
DECLARE_DELEGATE_OneParam(FFunKStageTickFunction, float);

// Structure that holds the references & configuration to one stages
USTRUCT(BlueprintType)
struct FUNK_API FFunKStage
{
	GENERATED_BODY()

public:
	FName Name;
	FFunKStageFunction StartDelegate;
	FFunKStageTickFunction TickDelegate;
	FFunKTimeLimit TimeLimit;
	bool IsOnStandalone = true;
	bool IsOnDedicatedServer = true;
	bool IsOnDedicatedServerClient = true;
	bool IsOnListenServer = true;
	bool IsOnListenServerClient = true;
	bool IsLatent;
};

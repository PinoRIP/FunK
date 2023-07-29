// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTimeLimit.h"
#include "FunKStage.generated.h"

DECLARE_DELEGATE(FFunKStageFunction);
DECLARE_DYNAMIC_DELEGATE(FFunKStageFunctions);

USTRUCT(BlueprintType)
struct FUNK_API FFunKStage
{
	GENERATED_BODY()

public:
	FFunKTimeLimit TimeLimit;
	bool IsOnStandalone = true;
	bool IsOnDedicatedServer = true;
	bool IsOnDedicatedServerClient = true;
	bool IsOnListenServer = true;
	bool IsOnListenServerClient = true;
	bool IsLatent;
	FFunKStageFunction Delegate;
	FName Name;
};

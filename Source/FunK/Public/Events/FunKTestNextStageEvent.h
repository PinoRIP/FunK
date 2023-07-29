// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestNextStageEvent.generated.h"

USTRUCT()
struct FFunKTestNextStageEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid TestRunID;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Seed;

	UPROPERTY()
	int32 StageIndex;
};

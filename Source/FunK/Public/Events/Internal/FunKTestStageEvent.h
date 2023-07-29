// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestStageEvent.generated.h"

class AFunKTestBase;

USTRUCT()
struct FFunKTestStageEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID;

	UPROPERTY()
	int32 Seed = 0;

	UPROPERTY()
	int32 StageIndex = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};
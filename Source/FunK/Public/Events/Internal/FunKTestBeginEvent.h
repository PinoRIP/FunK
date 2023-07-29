// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "UObject/Object.h"
#include "FunKTestBeginEvent.generated.h"

USTRUCT()
struct FFunKTestBeginEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	int32 Seed = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};

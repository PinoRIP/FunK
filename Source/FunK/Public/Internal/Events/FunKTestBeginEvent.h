// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestBeginEvent.generated.h"

class AFunKTestBase;

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

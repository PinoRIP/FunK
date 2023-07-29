// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "UObject/Object.h"
#include "FunKTestFinishEvent.generated.h"

USTRUCT()
struct FFunKTestFinishEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY()
	EFunKTestResult Result = EFunKTestResult::None;

	UPROPERTY()
	FString Message;
};


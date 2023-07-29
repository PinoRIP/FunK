// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "FunKTestFinishedEvent.generated.h"

class AFunKTestBase;

USTRUCT()
struct FFunKTestFinishedEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY()
	EFunKTestResult Result = EFunKTestResult::None;

	UPROPERTY()
	FString Message;
};

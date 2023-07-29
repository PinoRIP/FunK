// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "FunKTestStageFinishedEvent.generated.h"

class AFunKTestBase;

USTRUCT()
struct FFunKTestStageFinishedEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID;

	UPROPERTY()
	int32 StageIndex = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY()
	EFunKStageResult Result = EFunKStageResult::None;

	UPROPERTY()
	FString Message;

	UPROPERTY()
	int32 PeerIndex = 0;
};

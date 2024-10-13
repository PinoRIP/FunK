// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/FunKTestResult.h"
#include "UObject/Object.h"
#include "FunKTestEvents.generated.h"

class UFunKTestFragment;
class AFunKTestBase;

// Event that is used to begin a test
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
	int32 Variation = 0;
	
	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};

// Event that is used to request the beginning of a test
USTRUCT()
struct FFunKTestRequestBeginEvent : public FFunKTestBeginEvent
{
	GENERATED_BODY()
};

// Event that is used to begin a stage
USTRUCT()
struct FFunKTestStageBeginEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	int32 Stage = 0;
	
	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};

// Event that is used to finish a test
USTRUCT()
struct FFunKTestFinishEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	EFunKTestResult Result = EFunKTestResult::None;

	UPROPERTY()
	FString Message;
	
	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};

// Event that is used to request the finishing of a test
USTRUCT()
struct FFunKTestRequestFinishEvent : public FFunKTestFinishEvent
{
	GENERATED_BODY()
};

// Event that is used to finish a stage
USTRUCT()
struct FFunKTestStageFinishEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	int32 Stage = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY()
	EFunKStageResult Result = EFunKStageResult::None;

	UPROPERTY()
	FString Message;

	UPROPERTY()
	int32 PeerIndex = 0;
};

// Event that is used to sync networked fragments
USTRUCT()
struct FFunKTestNetworkedFragmentsCreatedEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TestRunID = 0;

	UPROPERTY()
	int32 Stage = 0;

	UPROPERTY()
	AFunKTestBase* Test = nullptr;

	UPROPERTY() 
	TArray<UFunKTestFragment*> Fragments;
};
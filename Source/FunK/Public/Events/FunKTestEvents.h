// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestResult.h"
#include "UObject/Object.h"
#include "FunKTestEvents.generated.h"

class UFunKTestFragment;
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
	int32 Variation = 0;
	
	UPROPERTY()
	AFunKTestBase* Test = nullptr;
};

USTRUCT()
struct FFunKTestRequestBeginEvent : public FFunKTestBeginEvent
{
	GENERATED_BODY()
};

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

USTRUCT()
struct FFunKTestRequestFinishEvent : public FFunKTestFinishEvent
{
	GENERATED_BODY()
};

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
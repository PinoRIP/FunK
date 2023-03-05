// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestInstructions.h"
#include "FunKTestRunnerType.h"
#include "UObject/Object.h"
#include "FunKTestRunner.generated.h"

struct FFunKTestInstructions;
class UFunKSink;
class UFunKEngineSubsystem;
class FFunKAutomationEntry;

USTRUCT()
struct FFunKTestRunnerState
{
	GENERATED_BODY()

	FFunKTestInstructions CurrentInstructions;
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKTestRunner : public UObject
{
	GENERATED_BODY()

public:
	virtual void Init(UFunKEngineSubsystem* FunKEngineSubsystem, EFunKTestRunnerType RunType);

	virtual bool Prepare(const FFunKTestInstructions& Instructions);
	
	virtual void Start();
	virtual bool Next();
	virtual void End();

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const;

	bool IsRunning() const { return IsStarted; }
	EFunKTestRunnerType GetType() const { return Type; }
	UFunKEngineSubsystem* GetSubsystem() const;
	const FString& GetParameter();
protected:
	virtual void RaiseStartEvent();
	virtual void GetSinks(TArray<TSubclassOf<UFunKSink>>& outSinks);

	virtual UFunKSink* NewSink(TSubclassOf<UFunKSink> sinkType);
	
private:
	bool IsStarted;
	EFunKTestRunnerType Type = EFunKTestRunnerType::None;
	FFunKTestInstructions ActiveTestInstructions;

	UPROPERTY()
	TArray<UFunKSink*> Sinks;

	TWeakObjectPtr<UFunKEngineSubsystem> FunKEngineSubsystem;
	
	virtual bool StartEnvironment(const FFunKTestInstructions& Instructions);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestInstructions.h"
#include "FunKTestRunnerType.h"
#include "Sinks/FunKSink.h"
#include "UObject/Object.h"
#include "FunKTestRunner.generated.h"

class AFunKWorldTestController;
struct FFunKTestInstructions;
class UFunKSink;
class UFunKEngineSubsystem;
class FFunKAutomationEntry;

UENUM(BlueprintType)
enum class EFunKTestRunnerState : uint8
{
	None,
	Canceled,
	Initialized,
	Ended,
	Started,
	WaitingForWorld,
	WaitingForConnections,
	Ready,
	ExecutingTest,
	EvaluatingTest
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKTestRunner : public UObject, public IFunKSink
{
	GENERATED_BODY()

public:
	virtual void Init(UFunKEngineSubsystem* FunKEngineSubsystem, EFunKTestRunnerType RunType);
	
	virtual void Start();
	virtual bool Test(const FFunKTestInstructions& Instructions);
	virtual void End();

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	bool IsRunning() const;
	bool IsWaitingForMap() const;
	EFunKTestRunnerType GetType() const { return Type; }
	UFunKEngineSubsystem* GetSubsystem() const;
	const FString& GetParameter();

	bool SetWorld(UWorld* world);

	AFunKWorldTestController* GetCurrentWorldController() const;
	
protected:
	virtual void RaiseStartEvent();
	virtual void GetSinks(TArray<TScriptInterface<IFunKSink>>& outSinks);

	virtual void UpdateState(EFunKTestRunnerState newState);

	bool IsRunningTestUnderOneProcess = false;

	bool IsStandaloneTest() const;
	static bool IsStandaloneTest(const FFunKTestInstructions& Instructions);
	bool IsDedicatedServerTest() const;
	static bool IsDedicatedServerTest(const FFunKTestInstructions& Instructions);
	bool IsListenServerTest() const;
	static bool IsListenServerTest(const FFunKTestInstructions& Instructions);
	
	bool IsDifferentEnvironment(const FFunKTestInstructions& Instructions) const;

	virtual TSubclassOf<AFunKWorldTestController> GetWorldControllerClass() const;
	
private:
	EFunKTestRunnerType Type = EFunKTestRunnerType::None;
	FFunKTestInstructions ActiveTestInstructions;
	EFunKTestRunnerState State = EFunKTestRunnerState::None;

	UPROPERTY()
	UWorld* CurrentTestWorld = nullptr;

	UPROPERTY()
	TArray<TScriptInterface<IFunKSink>> Sinks;

	TWeakObjectPtr<UFunKEngineSubsystem> FunKEngineSubsystem;
	
	virtual bool StartEnvironment(const FFunKTestInstructions& Instructions);
};

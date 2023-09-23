// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestInstructions.h"
#include "FunKTestRunnerType.h"
#include "Internal/EventBus/FunKEventBusRegistrationContainer.h"
#include "UObject/Object.h"
#include "FunKTestRunner.generated.h"

struct FFunKEvent;
class IFunKEnvironmentHandler;
class AFunKWorldTestController;
struct FFunKTestInstructions;
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
	EnvironmentSetup,
	Ready,
	ExecutingTest,
	EvaluatingTest
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
	
	virtual bool Start(const FFunKTestInstructions& Instructions);
	virtual bool Update();
	virtual void End();

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const;

	bool IsRunning() const;
	EFunKTestRunnerType GetType() const { return Type; }
	UFunKEngineSubsystem* GetSubsystem() const;

	AFunKWorldTestController* GetCurrentWorldController() const;
	
protected:
	virtual void UpdateState(EFunKTestRunnerState newState);
	
private:
	EFunKTestRunnerType Type = EFunKTestRunnerType::None;
	FFunKTestInstructions ActiveTestInstructions;
	EFunKTestRunnerState State = EFunKTestRunnerState::None;

	UPROPERTY()
	TScriptInterface<IFunKEnvironmentHandler> EnvironmentHandler;
	FFunKEventBusRegistrationContainer EventBusRegistration;
	TWeakObjectPtr<UFunKEngineSubsystem> FunKEngineSubsystem;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRunnerType.h"
#include "Sinks/FunKSink.h"
#include "FunKEngineSubsystem.generated.h"

class UFunKSink;
class UFunKTestRunner;
class FFunKAutomationEntry;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFunKTestRunner* StartTestRunner();
	UFunKTestRunner* ConnectTestRunner(bool isRemote);

	UFunKTestRunner* GetTestRunner();
	
	void EndTestRun();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	bool IsRunning() const;

private:
	UPROPERTY()
	UFunKTestRunner* ActiveTestRun = nullptr;
	UFunKTestRunner* SetupTestRun(EFunKTestRunnerType RunType);
};

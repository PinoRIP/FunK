// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRunnerType.h"
#include "FunKEngineSubsystem.generated.h"

class UFunKSink;
class UFunKTestRunner;
class FFunKAutomationEntry;
class AFunKWorldTestController;

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
	UFunKTestRunner* GetTestRunner() const;

	bool HasTestRunner() const;
	
	void EndTestRun();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	bool IsRunning() const;

	static bool IsSeparateTestingProcess();

	static void FunKDebug(FString msg);

private:
	UPROPERTY()
	UFunKTestRunner* ActiveTestRun = nullptr;
	UFunKTestRunner* SetupTestRun(EFunKTestRunnerType RunType);
};

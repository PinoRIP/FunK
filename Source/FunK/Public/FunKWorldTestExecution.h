// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRunID.h"
#include "Sinks/FunKSink.h"
#include "UObject/Object.h"
#include "FunKWorldTestExecution.generated.h"

class AFunKWorldTestController;
class AFunKTestBase;
class UFunKSettingsObject;
struct FFunKTimeLimit;

USTRUCT()
struct FFunKTestExecutionState
{
	GENERATED_BODY()

public:
	FFunKTestExecutionState()
		: Id(FFunKTestRunID())
		, LastStartedStage(INDEX_NONE)
		, LastFinishedStage(INDEX_NONE)
		, IsExecutionFinished(false)
		, NetMode(ENetMode::NM_MAX)
		, Controller(nullptr)
	{ }
	
	FFunKTestRunID Id;

	int32 LastStartedStage;
	int32 LastFinishedStage;
	bool IsExecutionFinished;

	ENetMode NetMode;
	
	UPROPERTY()
	AFunKWorldTestController* Controller;
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKWorldTestExecution : public UObject, public FTickableGameObject, public IFunKSink
{
	GENERATED_BODY()

public:
	void Start(const UWorld* world, const TArray<AFunKTestBase*>& testsToExecute, TScriptInterface<IFunKSink> reportSink, FFunKTestRunID executionId);

	AFunKWorldTestController* GetMasterController() const;

	bool IsFinished() const;
	
private:
	UPROPERTY()
	TScriptInterface<IFunKSink> ReportSink;

	UPROPERTY()
	int32 CurrentTestIndex = -1;
	int32 CurrentStageIndex = -1;
	
	UPROPERTY()
	AFunKWorldTestController* MasterController = nullptr;

	UPROPERTY()
	TArray<FFunKTestExecutionState> CurrentExecutions;
	FString ThisExecutionID;

	UPROPERTY()
	TArray<AFunKTestBase*> TestsToExecute;

	uint32 LastTickFrame = INDEX_NONE;

	UPROPERTY()
	const UFunKSettingsObject* Settings = nullptr;

	bool IsAllFinished = false;
	bool IsAnyStarted = false;

	bool PendingNextStage = false;
	bool PendingNextTest = false;

	void NextTest();
	void NextTestAsync();
	void RunTest(AFunKTestBase* test);
	void RunTestOnController(AFunKTestBase* test, AFunKWorldTestController* controller, ENetMode netMode);
	void NextStage();
	void NextStageAsync();

	void OnTestStageFinished(const FFunKEvent& raisedEvent);
	void OnTestStageFinished(FFunKTestExecutionState* State);
	void OnTestStageFinished();

	void OnTestFinished(const FFunKEvent& raisedEvent);
	void OnTestFinished(FFunKTestExecutionState* State);
	void OnTestFinished();
	
	void OnTestExecutionCanceled(const FString& ReasonMessage);
	
	void Finish();

	void StartSync();

	static bool IsTimeout(const FFunKTimeLimit* limit, float time);
	AFunKTestBase* GetCurrentTest();
	FFunKTestExecutionState* GetStateFromEvent(const FFunKEvent& raisedEvent);

public:
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;

private: //STATS
	float TotalTime;
	float PendingStageTime;
	float PendingSyncTime;

public:
	static FString FunKTestLifeTimeBeginEvent;
	static FString FunKTestLifeTimeBeginStageEvent;
	static FString FunKTestLifeTimeFinishStageEvent;
	static FString FunKTestLifeTimeFinishEvent;
	static FString FunKTestLifeTimeAllTestExecutionsFinishedEvent;
};

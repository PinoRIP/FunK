// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sinks/FunKSink.h"
#include "UObject/Object.h"
#include "FunKWorldTestExecution.generated.h"

class AFunKWorldTestController;
class AFunKTestBase;
struct FFunKTimeLimit;

typedef FGuid FFunKTestID;

struct FFunKTestExecutionHandle
{
	
};

USTRUCT()
struct FFunKWorldTestState
{
	GENERATED_BODY()

public:
	FFunKWorldTestState()
		: Id(FGuid())
		, Controller(nullptr)
		, SetupCompleteTime(-1)
		, FinishedTime(-1)
	{ }
	
	FFunKTestID Id;

	UPROPERTY()
	AFunKWorldTestController* Controller;

	int32 SetupCompleteTime;
	int32 FinishedTime;

	bool IsSetup() const
	{
		return SetupCompleteTime >= 0;
	}

	bool IsFinished() const
	{
		return FinishedTime >= 0;
	}
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKWorldTestExecution : public UObject, public IFunKSink
{
	GENERATED_BODY()

public:
	void Start(UWorld* world, const TArray<AFunKTestBase*>& testsToExecute, TScriptInterface<IFunKSink> reportSink, FFunKTestID executionId);
	void Update(float DeltaTime);

	bool IsFinished() const;

	AFunKWorldTestController* GetMasterController() const;

	static bool IsExecutionFinishedEvent(const FFunKEvent& raisedEvent, FFunKTestID ExecutionId);
	
private:
	UPROPERTY()
	TScriptInterface<IFunKSink> ReportSink;

	UPROPERTY()
	int32 CurrentTest = -1;
	
	UPROPERTY()
	AFunKWorldTestController* MasterController = nullptr;

	UPROPERTY()
	TArray<FFunKWorldTestState> CurrentExecutions;
	FString ThisExecutionId;

	UPROPERTY()
	TArray<AFunKTestBase*> TestsToExecute;

	bool IsAllFinished = false;

	void NextTest();
	void Finish();

	void RunTest(AFunKTestBase* test);

	bool HandleTimeout(AFunKTestBase* currentTest, const FFunKTimeLimit* limit, float time);
	static bool IsTimeout(const FFunKTimeLimit* limit, float time);

	FFunKWorldTestState& RunTestOnController(AFunKTestBase* test, AFunKWorldTestController* controller);
	AFunKTestBase* GetCurrentTest();

	FFunKWorldTestState* GetStateFromEvent(const FFunKEvent& raisedEvent);

	void OnTestPreparationComplete(const FFunKEvent& raisedEvent);
	void OnAllTestsPreparationsComplete();

	void OnTestExecutionComplete(const FFunKEvent& raisedEvent);
	void OnAllTestsExecutionsComplete();

public:
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

private: //STATS
	float TotalTime;
	float PreparationTime;
	float ExecutionTime;
	float PendingNetworkingTime;

public:
	static FString FunKTestLifeTimeStartEvent;
	static FString FunKTestLifeTimePreparationCompleteEvent;
	static FString FunKTestLifeTimeTestFinishedEvent;
	static FString FunKTestLifeTimeTestExecutionFinishedEvent;
};

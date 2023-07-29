// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sinks/FunKSink.h"
#include "UObject/Object.h"
#include "FunKWorldTestExecution.generated.h"

class AFunKWorldTestController;
class AFunKFunctionalTest;

typedef FGuid FFunKTestId;

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
	
	FFunKTestId Id;

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
	void Start(UWorld* world, const TArray<AFunKFunctionalTest*>& testsToExecute, TScriptInterface<IFunKSink> reportSink, FFunKTestId executionId);
	void Update(float DeltaTime);

	bool IsFinished() const;

	AFunKWorldTestController* GetMasterController() const;

	static bool IsExecutionFinishedEvent(const FFunKEvent& raisedEvent, FFunKTestId ExecutionId);
	
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
	TArray<AFunKFunctionalTest*> TestsToExecute;

	bool IsAllFinished = false;

	void NextTest();
	void Finish();

	void RunTest(AFunKFunctionalTest* test);

	void CheckPreparationsTimeout(struct FFunKTimeLimit& limit, float time, const FString msg);

	FFunKWorldTestState& RunTestOnController(AFunKFunctionalTest* test, AFunKWorldTestController* controller);
	AFunKFunctionalTest* GetCurrentTest();

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
	float PreparationNetworkingTime;
	float ExecutionTime;
	float FinishedNetworkingTime;

public:
	static FString FunKTestLifeTimeStartEvent;
	static FString FunKTestLifeTimePreparationCompleteEvent;
	static FString FunKTestLifeTimeTestFinishedEvent;
	static FString FunKTestLifeTimeTestExecutionFinishedEvent;
};

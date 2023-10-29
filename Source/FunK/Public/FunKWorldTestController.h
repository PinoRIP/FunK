// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKWorldTestController.generated.h"

class UFunKSettingsObject;
class AFunKTestBase;
class UFunKTestRunner;
class UFunKWorldTestExecution;

/*
 * Controller that executes the given tests in the world it has been spawned in.
 */
UCLASS()
class FUNK_API AFunKWorldTestController : public AActor
{
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	AFunKWorldTestController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Executes the test by name
	virtual void ExecuteTestByName(FString TestName);

	// Executes all test in the world the controller has been spawned in
	virtual void ExecuteAllTests();

	// true when all test executions have ended 
	bool IsFinished() const;
	
private:
	UPROPERTY()
	int32 TestRunID;

	UPROPERTY()
	int32 Seed;

	UPROPERTY()
	int32 CurrentVariation = 0;

	UPROPERTY()
	TArray<AFunKTestBase*> Tests;

	UPROPERTY()
	AFunKTestBase* CurrentTest;

	UPROPERTY()
	AFunKTestBase* LastTest;

	UPROPERTY()
	const UFunKSettingsObject* Settings = nullptr;

	bool IsWaitingForTimeout = false;

	float CurrentTestStageWaitingTime;
	
	void ExecuteTests();
	bool Next();
	void End();
	
	void LogMissingTests(const TArray<FString>& MissingTests) const;
};

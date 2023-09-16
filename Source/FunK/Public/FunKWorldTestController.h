// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKWorldTestController.generated.h"

class UFunKSettingsObject;
class AFunKTestBase;
class UFunKTestRunner;
class UFunKWorldTestExecution;

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

	virtual void ExecuteTestByName(FString TestName);
	virtual void ExecuteAllTests();

	virtual bool IsFinished() const;
	
private:
	UPROPERTY()
	int32 TestRunID;

	UPROPERTY()
	int32 Seed;

	UPROPERTY()
	TArray<AFunKTestBase*> Tests;

	UPROPERTY()
	AFunKTestBase* CurrentTest;

	UPROPERTY()
	const UFunKSettingsObject* Settings = nullptr;

	bool IsWaitingForTimeout = false;

	float CurrentTestStageWaitingTime;
	
	void ExecuteTests();
	bool Next();
	void End();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Events/FunKEvent.h"
#include "FunKWorldTestController.generated.h"

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

	virtual void BeginDestroy() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ExecuteTestByName(FString TestName);
	virtual void ExecuteAllTests();

	virtual bool IsFinished() const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const;
private:
	FString RemoteTestRunID;
	
	UPROPERTY()
	UFunKWorldTestExecution* CurrentTestExecution;
	
	void ExecuteTests(const TArray<AFunKTestBase*>& TestToExecute, FGuid TestRunID);

	UFUNCTION(Server, Reliable)
	void ServerExecuteTestByName(const FString& TestName, FGuid TestRunID);
	UFUNCTION(Server, Reliable)
	void ServerExecuteAllTests(FGuid TestRunID);
	
	virtual void ExecuteTestByName(const FString& TestName, FGuid TestRunID);
	virtual void ExecuteAllTests(FGuid TestRunID);
	
	UFUNCTION(Client, Reliable)
	void ClientBeginLocalTest(AFunKTestBase* TestToBegin, FGuid TestRunID, int32 Seed);
	void BeginLocalTest(AFunKTestBase* TestToBegin, FGuid TestRunID, int32 Seed);
	
	UFUNCTION(Client, Reliable)
	void ClientBeginLocalTestStage(AFunKTestBase* Test, int32 StageIndex);
	void BeginLocalTestStage(AFunKTestBase* Test, int32 StageIndex);
	
	UFUNCTION(Client, Reliable)
	void ClientFinishLocalTest(AFunKTestBase* TestToCancel, EFunKTestResult Result, const FString& Message);
	void FinishLocalTest(AFunKTestBase* TestToCancel, EFunKTestResult Result, const FString& Message);

	UFUNCTION(Server, Reliable)
	virtual void ServerSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;
	UFUNCTION(Client, Reliable)
	virtual void ClientSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;
	
private:
	friend UFunKWorldTestExecution;
};

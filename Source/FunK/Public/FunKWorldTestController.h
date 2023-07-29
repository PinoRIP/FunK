// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKWorldTestController.generated.h"

class AFunKFunctionalTest;
class UFunKTestRunner;

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

	virtual void CreateTestControllerForClient(APlayerController* NewPlayer);

	virtual void BeginDestroy() override;

	virtual void OnRep_Owner() override;
	
	void ExecuteTest(AFunKFunctionalTest* TestToExecute);

	void FinishedCurrentTest();
	
private:
	UPROPERTY()
	UFunKTestRunner* TestRunner = nullptr;

	UPROPERTY()
	TArray<AFunKWorldTestController*> SpawnedController;

	UPROPERTY()
	AFunKWorldTestController* Creator = nullptr;

	UPROPERTY()
	AFunKWorldTestController* TestOrigin = nullptr;
	
	UPROPERTY()
	AFunKFunctionalTest* CurrentTest = nullptr;

	void OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer);

	bool IsRunning = false;

	UFUNCTION(Server, Reliable)
	void ServerExecuteTest(AFunKFunctionalTest* TestToExecute);
	
	UFUNCTION(Client, Reliable)
	void ClientExecuteTest(AFunKFunctionalTest* TestToExecute);
	
	virtual void SendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;
	virtual void ApplySendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;

	UFUNCTION(Server, Reliable)
	virtual void ServerSendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;
	
	UFUNCTION(Client, Reliable)
	virtual void ClientSendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetTestRunner(UFunKTestRunner* testRunner);

	virtual void ExecuteTestByName(FString TestName);
	virtual void ExecuteAllTests();

	virtual bool IsFinished();

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;

	friend UFunKExtAutomationSink;
};

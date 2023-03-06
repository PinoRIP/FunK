// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sinks/FunKSink.h"
#include "FunKWorldTestController.generated.h"

class AFunKFunctionalTest;
class UFunKTestRunner;

UCLASS()
class FUNK_API AFunKWorldTestController : public AActor, public IFunKSink
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
	
	void ExecuteTest(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> ReportSink);

	void FinishedCurrentTest();
	
private:
	UPROPERTY()
	TScriptInterface<IFunKSink> ReportSink;

	UPROPERTY()
	TArray<AFunKWorldTestController*> SpawnedController;
	
	UPROPERTY()
	AFunKFunctionalTest* CurrentTest = nullptr;

	void OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer);

	UFUNCTION(Server, Reliable)
	void ServerExecuteTest(AFunKFunctionalTest* TestToExecute);
	
	UFUNCTION(Client, Reliable)
	void ClientExecuteTest(AFunKFunctionalTest* TestToExecute);
	
	void ExecuteTestRun(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> ReportSink);
	
	virtual void SendEvent(const FFunKEvent& raisedEvent) const;
	virtual void ApplySendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;

	UFUNCTION(Server, Reliable)
	virtual void ServerSendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;
	
	UFUNCTION(Client, Reliable)
	virtual void ClientSendEvent(EFunKEventType eventType, const FString& Message, const FString& Context) const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> ReportSink);
	virtual void ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink);

	virtual bool IsFinished() const;

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;
};

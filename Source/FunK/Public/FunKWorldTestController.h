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

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CreateTestControllerForClient(APlayerController* NewPlayer);

	virtual void BeginDestroy() override;

	virtual void OnRep_Owner() override;
	
	void ExecuteTest(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> ReportSink, FGuid executionId);

	void FinishedCurrentLocalTest();
	void FinishedCurrentTest();

	virtual void ControllerReady();
	
private:
	UPROPERTY()
	TScriptInterface<IFunKSink> ReportSink;

	UPROPERTY()
	TArray<AFunKWorldTestController*> SpawnedController;
	
	UPROPERTY()
	AFunKFunctionalTest* CurrentTest = nullptr;
	FGuid CurrentExecutionId;
	TArray<FGuid> SubExecutionIds;
	
	float TotalTime;
	float NetworkingTotalTime;

	UPROPERTY( replicated )
	int32 ActiveController;

	UPROPERTY( replicated )
	int32 ControllerIndex;

	bool IsControllerReadinessSend = false;
	bool IsControllerLocallyReady() const;
	void CheckControllerReady();

	void OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer);

	UFUNCTION(Server, Reliable)
	void ServerExecuteTest(AFunKFunctionalTest* TestToExecute, FGuid ExecutionId);
	
	UFUNCTION(Client, Reliable)
	void ClientExecuteTest(AFunKFunctionalTest* TestToExecute, FGuid ExecutionId);
	
	void ExecuteTestRun(AFunKFunctionalTest* TestToExecute, TScriptInterface<IFunKSink> ReportSink, FGuid ExecutionId);
	
	virtual void SendEvent(const FFunKEvent& raisedEvent) const;
	virtual void ApplySendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;

	UFUNCTION(Server, Reliable)
	virtual void ServerSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;
	
	UFUNCTION(Client, Reliable)
	virtual void ClientSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;

	UFUNCTION(Server, Reliable)
	virtual void ServerControllerReady() const;

	bool ReportEvent(const FFunKEvent& raisedEvent) const;
	void FulfillSubExecution(const FFunKEvent& raisedEvent) const;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> ReportSink);
	virtual void ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink);

	virtual bool IsFinished() const;
	virtual bool IsLocallyFinished() const;

	virtual void RaiseInfoEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseWarningEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseErrorEvent(const FString& Message, const FString& Context = "") const;
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	int32 GetActiveControllerCount() const;
	int32 GetControllerIndex() const;

	bool IsLocalTestController() const;

	virtual void SetupLocalTestController();
};

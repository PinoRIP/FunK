// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKWorldTestExecution.h"
#include "GameFramework/Actor.h"
#include "Sinks/FunKSink.h"
#include "FunKWorldTestController.generated.h"

class AFunKTestBase;
class UFunKTestRunner;

UCLASS()
class FUNK_API AFunKWorldTestController : public AActor, public IFunKSink
{
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	AFunKWorldTestController();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	int32 GetActiveControllerCount() const;
	int32 GetControllerIndex() const;

	bool IsLocalTestController() const;

	virtual void SetupLocalTestController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CreateTestControllerForClient(APlayerController* NewPlayer);

	virtual void BeginDestroy() override;

	virtual void OnRep_Owner() override;

	virtual void ControllerReady();
	
private:
	UPROPERTY( replicated )
	int32 ActiveController;

	UPROPERTY( replicated )
	int32 ControllerIndex = INDEX_NONE;

	bool IsControllerReadinessSend = false;
	bool IsControllerLocallyReady() const;
	void CheckControllerReady();

	void OnConnection(AGameModeBase* GameMode, APlayerController* NewPlayer);

	UFUNCTION(Server, Reliable)
	virtual void ServerControllerReady() const;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ExecuteTestByName(FString TestName, TScriptInterface<IFunKSink> ReportSink);
	virtual void ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink);

	virtual bool IsFinished() const;
	
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	FString GetRoleName() const;

private:
	UPROPERTY()
	TScriptInterface<IFunKSink> LocalReportSink;
	FString ExpectedProxyEvent;
	
	UPROPERTY()
	TArray<AFunKWorldTestController*> SpawnedController;
	
	UPROPERTY()
	UFunKWorldTestExecution* CurrentTestExecution;
	
	void ExecuteTests(const TArray<AFunKTestBase*>& TestToExecute, TScriptInterface<IFunKSink> ReportSink, FGuid executionId);

	UFUNCTION(Server, Reliable)
	void ServerExecuteTestByName(const FString& TestName, FGuid ExecutionId);
	UFUNCTION(Server, Reliable)
	void ServerExecuteAllTests(FGuid ExecutionId);
	
	virtual void ExecuteTestByName(const FString& TestName, TScriptInterface<IFunKSink> ReportSink, FGuid ExecutionId);
	virtual void ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink, FGuid ExecutionId);
	
	UFUNCTION(Client, Reliable)
	void ClientBeginLocalTestSetup(AFunKTestBase* TestToExecute, FGuid ExecutionId);
	void BeginLocalTestSetup(AFunKTestBase* TestToExecute, FGuid ExecutionId);
	
	UFUNCTION(Client, Reliable)
	void ClientBeginLocalTestExecution(AFunKTestBase* TestToExecute);
	void BeginLocalTestExecution(AFunKTestBase* TestToExecute);
	
	UFUNCTION(Client, Reliable)
	void ClientFinishLocalTest(AFunKTestBase* TestToCancel, EFunKFunctionalTestResult Result, const FString& Message);
	void FinishLocalTest(AFunKTestBase* TestToCancel, EFunKFunctionalTestResult Result, const FString& Message);

	UFUNCTION(Server, Reliable)
	virtual void ServerSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;
	UFUNCTION(Client, Reliable)
	virtual void ClientSendEvent(EFunKEventType eventType, const FString& Message, const TArray<FString>& Context) const;
private:
	friend class UFunKWorldTestExecution;
};

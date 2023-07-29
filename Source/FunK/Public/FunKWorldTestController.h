// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sinks/FunKSink.h"
#include "FunKTestRunID.h"
#include "FunKWorldTestController.generated.h"

class AFunKTestBase;
class UFunKTestRunner;
class UFunKWorldTestExecution;

UCLASS()
class FUNK_API AFunKWorldTestController : public AActor, public IFunKSink
{
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	AFunKWorldTestController();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	int32 GetActiveControllerCount() const;
	int32 GetControllerNumber() const;

	bool IsLocalTestController() const;

	virtual void SetupLocalTestController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CreateTestControllerForClient(APlayerController* NewPlayer);

	virtual void BeginDestroy() override;

	virtual void OnRep_Owner() override;

	virtual void OnControllerReady();
	
private:
	UPROPERTY( replicated )
	int32 ActiveController;

	UPROPERTY( replicated )
	int32 ControllerNumber = INDEX_NONE;

	UPROPERTY( replicated )
	bool IsServerDedicated = false;

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
	bool GetIsServerDedicated() const { return IsServerDedicated; }

	const TArray<AFunKWorldTestController*>& GetSpawnedController() const { return SpawnedController; }

private:
	UPROPERTY()
	TScriptInterface<IFunKSink> LocalReportSink;
	FString RemoteTestRunID;
	
	UPROPERTY()
	TArray<AFunKWorldTestController*> SpawnedController;
	
	UPROPERTY()
	UFunKWorldTestExecution* CurrentTestExecution;
	
	void ExecuteTests(const TArray<AFunKTestBase*>& TestToExecute, TScriptInterface<IFunKSink> ReportSink, FFunKTestRunID TestRunID);

	UFUNCTION(Server, Reliable)
	void ServerExecuteTestByName(const FString& TestName, FGuid TestRunID);
	UFUNCTION(Server, Reliable)
	void ServerExecuteAllTests(FGuid TestRunID);
	
	virtual void ExecuteTestByName(const FString& TestName, TScriptInterface<IFunKSink> ReportSink, FFunKTestRunID TestRunID);
	virtual void ExecuteAllTests(TScriptInterface<IFunKSink> ReportSink, FFunKTestRunID TestRunID);
	
	UFUNCTION(Client, Reliable)
	void ClientBeginLocalTest(AFunKTestBase* TestToBegin, FGuid TestRunID);
	void BeginLocalTest(AFunKTestBase* TestToBegin, FFunKTestRunID TestRunID);
	
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

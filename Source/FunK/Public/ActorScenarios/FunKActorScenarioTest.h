// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestBase.h"
#include "FunKTypes.h"
#include "GameFramework/Actor.h"
#include "FunKActorScenarioTest.generated.h"

UCLASS(Blueprintable)
class FUNK_API AFunKActorScenarioTest : public AFunKTestBase
{
	GENERATED_BODY()

public:
	AFunKActorScenarioTest();
	
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	virtual void OnBeginStage(const FName& StageName) override;
	virtual void OnFinish(const FString& Message) override;

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeScenarioTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ActTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit AssertTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "/Script/FunK.EFunKTestLocationTarget"), Category="FunK|Setup")
	int32 AlsoArrangeOn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "/Script/FunK.EFunKTestLocationTarget"), Category="FunK|Setup")
	int32 AlsoActOn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "/Script/FunK.EFunKTestLocationTarget"), Category="FunK|Setup")
	int32 AlsoAssertOn = 0;

	virtual void SetupStages(FFunKStagesSetup& stages) override final;

	void InvokeAssume();
	void InvokeArrange();
	void InvokeAssert();

	bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	bool BpAssume();

	void ArrangeScenario();
	void CheckArrangeScenarioFinish(float DeltaTime);

	bool Arrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Arrange")
	bool BpArrange();
	
	void Act();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Act")
	void BpAct();

	bool Assert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assert")
	bool BpAssert();

	bool HasMoreScenarios() const;
	virtual void OnStageScenarioChanged(const FName& StageName, const FString& NewStageScenario);

	virtual void FinishStage(EFunKTestResult TestResult, const FString& Message) override;

	UFUNCTION(BlueprintCallable, Category="FunK")
	AActor* GetAcquireActorByComponent(UFunKActorScenarioComponent* Component);

	bool IsSkippingClient2() const;

	bool IsArrangeAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	bool IsActAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	bool IsAssertAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	bool IsAlsoOn(int32 state, EFunKTestLocationTarget alsoOnTarget) const;
	
private:
	UPROPERTY( replicated )
	TArray<AActor*> AcquiredActors;

	FString CurrentStageScenario;
	bool IsCurrentStageScenarioFinished;

	void ErrorFallbackStage();
	void SetupStages(FFunKStagesSetup& stages, TArray<UFunKActorScenarioComponent*>& actorScenarioComponents);
	void AddScenarioStages(FFunKStagesSetup& stages, const FString& name, bool standalone, bool dedicated, bool dedicatedClient, bool listen, bool listenClient);

	void GetActorScenarioComponents(TArray<UFunKActorScenarioComponent*>& ActorScenarioComponents);
	FORCEINLINE FString ParseStageScenario(const FString& StageName);

	static FString FunKTestActorScenarioStageExtensionStandalone;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionListenServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToClient;
};

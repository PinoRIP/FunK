// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/FunKTestBase.h"
#include "Base/FunKTimeLimit.h"
#include "GameFramework/Actor.h"
#include "ActorScenarios/FunKTestLocationTarget.h"
#include "FunKActorScenarioTest.generated.h"

UCLASS(Blueprintable)
class FUNK_API AFunKActorScenarioTest : public AFunKTestBase
{
	GENERATED_BODY()

public:
	AFunKActorScenarioTest();
	
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	virtual void OnBeginStage() override;
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
	virtual bool IsExecutingStage(const FFunKStage& stage) const override;

	void InvokeAssume();

	bool Assume();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assume")
	bool BpAssume();

	void ArrangeScenario();
	void CheckArrangeScenarioFinish(float DeltaTime);

	void Arrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Arrange")
	void BpArrange();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ArrangeTick")
	void BpArrangeTick(float DeltaTime);
	
	void Act();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Act")
	void BpAct();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="ActTick")
	void BpActTick(float DeltaTime);

	void Assert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="Assert")
	void BpAssert();
	UFUNCTION(BlueprintNativeEvent, Category="FunK", DisplayName="AssertTick")
	void BpAssertTick(float DeltaTime);

	bool HasMoreScenarios() const;
	virtual void OnStageScenarioChanged(const FName& StageName, const FString& NewStageScenario);

	// virtual void FinishStage(EFunKTestResult TestResult, const FString& Message) override;

	UFUNCTION(BlueprintCallable, Category="FunK")
	AActor* GetAcquireActorByComponent(UFunKActorScenarioComponent* Component);

	bool IsSkippingClient2(const FFunKStage& stage) const;

	bool IsArrangeAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	bool IsActAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	bool IsAssertAlsoOn(EFunKTestLocationTarget alsoOnTarget) const;
	static bool IsAlsoOn(int32 state, EFunKTestLocationTarget alsoOnTarget);
	
private:
	UPROPERTY( replicated )
	TArray<AActor*> AcquiredActors;

	FString CurrentStageScenario;
	bool IsCurrentStageScenarioFinished;

	void ErrorFallbackStage();
	void SetupStages(FFunKStagesSetup& stages, TArray<UFunKActorScenarioComponent*>& actorScenarioComponents);
	void AddScenarioStages(FFunKStagesSetup& stages, const FString& name, bool standalone, bool dedicated, bool dedicatedClient, bool listen, bool listenClient);
	void AssignOwner(AActor* Actor, AActor* NewOwner, bool tryToPossess);

	void GetActorScenarioComponents(TArray<UFunKActorScenarioComponent*>& ActorScenarioComponents);
	FORCEINLINE FString ParseStageScenario(const FString& StageName) const;

	static FString FunKTestActorScenarioStageExtensionStandalone;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionListenServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToClient;
};

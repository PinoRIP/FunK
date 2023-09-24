// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Internal/ActorScenario/FunKActorScenario.h"
#include "Internal/FunKTestBase.h"
#include "Internal/Setup/FunKTimeLimit.h"
#include "GameFramework/Actor.h"
#include "FunKActorScenarioTest.generated.h"


struct FFunKStageSetupBase;
enum class EFunKTestEnvironmentType : uint8;
class UFunKActorScenarioComponent;

USTRUCT()
struct FFunKActorScenarioAlsoSetup
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool Arrange = false;

	UPROPERTY()
	bool Act = false;

	UPROPERTY()
	bool Assert = false;
};

USTRUCT()
struct FFunKActorSingleScenarioSetup
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool IsActive = true;

	UPROPERTY()
	bool IsAvailable = true;

	UPROPERTY()
	FFunKActorScenarioAlsoSetup OnOpposition = FFunKActorScenarioAlsoSetup();

	UPROPERTY()
	FFunKActorScenarioAlsoSetup OnThird = FFunKActorScenarioAlsoSetup();
};

USTRUCT()
struct FFunKActorScenarioMultiplayerSetup
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool IsActive = true;

	UPROPERTY()
	FFunKActorSingleScenarioSetup ServerToClient = FFunKActorSingleScenarioSetup();

	UPROPERTY()
	FFunKActorSingleScenarioSetup ClientToServer = FFunKActorSingleScenarioSetup();

	UPROPERTY()
	FFunKActorSingleScenarioSetup ClientToClient = FFunKActorSingleScenarioSetup();
};

USTRUCT()
struct FFunKActorScenarioSetup
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool TestStandalone = true;

	UPROPERTY()
	bool TestStandaloneAvailable = true;

	UPROPERTY()
	FFunKActorScenarioMultiplayerSetup TestDedicatedServer = FFunKActorScenarioMultiplayerSetup();

	UPROPERTY()
	FFunKActorScenarioMultiplayerSetup TestListenServer = FFunKActorScenarioMultiplayerSetup();
};

struct FFunKScenarioStages
{
	FFunKScenarioStages(FFunKStageSetupBase* InArrangeScenario, FFunKStageSetupBase* InArrange, FFunKStageSetupBase* InAct, FFunKStageSetupBase* InAssert)
		: ArrangeScenario(InArrangeScenario)
		, Arrange(InArrange)
		, Act(InAct)
		, Assert(InAssert)
	{
	}
	
	FFunKStageSetupBase* ArrangeScenario;
	FFunKStageSetupBase* Arrange;
	FFunKStageSetupBase* Act;
	FFunKStageSetupBase* Assert;
};

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

	UFUNCTION(BlueprintCallable)
	bool IsListenServerScenario() const;

	UFUNCTION(BlueprintCallable)
	bool IsDedicatedServerScenario() const;

	UFUNCTION(BlueprintCallable)
	bool IsServerToClientScenario() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsClientToServerScenario() const;

	UFUNCTION(BlueprintCallable)
	bool IsClientToClientScenario() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsStandaloneScenario() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsLocalOwnershipNone() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsLocalOwnershipAppositionPlayer() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsLocalOwnershipOppositionPlayer() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsLocalOwnershipAI() const;

	UFUNCTION()
	const FFunKActorScenarioSetup& GetScenarioSetup();
	
	void GetActorScenarioComponents(TArray<UFunKActorScenarioComponent*>& ActorScenarioComponents) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeScenarioTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ArrangeTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit ActTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FunK|Setup|Timeout")
	FFunKTimeLimit AssertTimeLimit;

	UPROPERTY()
	FFunKActorScenarioSetup ScenarioSetup = FFunKActorScenarioSetup();

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

	UFUNCTION(BlueprintCallable, Category="FunK")
	AActor* GetAcquireActorByComponent(UFunKActorScenarioComponent* Component);

	bool IsSkippingClient2(const FFunKStage& stage) const;
	
private:
	UPROPERTY( replicated )
	TArray<AActor*> AcquiredActors;

	FString CurrentStageScenario;
	FFunKActorScenario CurrentScenario;

	void ErrorFallbackStage();
	void SetupScenarioStages(FFunKStagesSetup& stages);
	FFunKScenarioStages AddScenarioStages(FFunKStagesSetup& stages, const FString& name);

	
	FORCEINLINE FString ParseStageScenario(const FString& StageName) const;

	static FString FunKTestActorScenarioStageExtensionStandalone;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionDedicatedServerClientToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToServer;
	static FString FunKTestActorScenarioStageExtensionListenServerServerToClient;
	static FString FunKTestActorScenarioStageExtensionListenServerClientToClient;
};

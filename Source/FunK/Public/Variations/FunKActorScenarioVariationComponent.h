// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRootVariationComponent.h"
#include "Components/ActorComponent.h"
#include "Util/FunKUtilTypes.h"
#include "FunKActorScenarioVariationComponent.generated.h"

UENUM(BlueprintType)
enum class EFunKActorScenarioVariationOwnership : uint8
{
	None				= 0,
	AppositionPlayer	= 1 << 0,
	OppositionPlayer	= 1 << 1,

	//TODO: Setup AI
	AI					= 1 << 2,
};
ENUM_CLASS_FLAGS(EFunKActorScenarioVariationOwnership);

UENUM(BlueprintType)
enum class EFunKActorScenarioMode : uint8
{
	ClientToClient,
	ClientToServer,
	ServerToClient,
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationSpawnActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EFunKActorScenarioVariationOwnership Ownership = EFunKActorScenarioVariationOwnership::None;
	
	UPROPERTY(EditAnywhere)
	AActor* SceneActor = nullptr;

	UPROPERTY(EditAnywhere)
	FFunKActorScenarioVariationSpawnActor SpawnActor;

	//TODO: Spawn handler?
};

struct FFunKOwnershipDistribution
{
	FFunKOwnershipDistribution(int32 InNoneCount, int32 InAppositionPlayerCount, int32 InOppositionPlayerCount, int32 InAICount)
		: NoneCount(InNoneCount)
		, AppositionPlayerCount(InAppositionPlayerCount)
		, OppositionPlayerCount(InOppositionPlayerCount)
		, AICount(InAICount)
	{  }
	
public:
	int32 NoneCount;
	int32 AppositionPlayerCount;
	int32 OppositionPlayerCount;
	int32 AICount;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKActorScenarioVariationComponent : public UFunKTestRootVariationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKActorScenarioVariationComponent();

private:
	UPROPERTY(EditAnywhere, Category="FunK|Setup")
	TArray<FFunKActorScenarioVariationActor> Actors;

public:
	virtual int32 GetCount() override;
	virtual void Begin(int32 index) override;
	virtual bool IsReady() override;
	virtual void Finish() override;
	virtual FString GetName() override;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

protected:
	virtual AActor* AcquireActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* GetSceneActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* SpawnActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual void AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership);
	virtual void ReleaseActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	virtual void ReleaseSceneActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	virtual void ReleaseSpawnActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	virtual APlayerController* GetController(EFunKActorScenarioVariationOwnership Ownership);

	EFunKTestEnvironmentType GetEnvironment() const;
	FFunKOwnershipDistribution GetOwnershipDistribution();
	
private:
	EFunKActorScenarioMode Mode;
	
	UPROPERTY( replicated )
	TArray<AActor*> AcquiredActors;

public:
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetActor(int32 Index = 0);

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetActorByOwnership(EFunKActorScenarioVariationOwnership Ownership, int32 Index = 0);

	UFUNCTION(BlueprintCallable)
	EFunKActorScenarioVariationOwnership GetLocalOwnerships();

	UFUNCTION(BlueprintCallable)
	bool IsAppositionPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsOppositionPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsAI();
};


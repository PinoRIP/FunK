// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRootVariationComponent.h"
#include "Components/ActorComponent.h"
#include "FunKActorScenarioVariationComponent.generated.h"

UENUM(BlueprintType)
enum class EFunKActorScenarioVariationOwnership : uint8
{
	None,
	AppositionPlayer,
	OppositionPlayer,

	//TODO: Setup AI
	AI,
};

UENUM(BlueprintType)
enum class EFunKActorScenarioMode : uint8
{
	Standalone,
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

private:
	EFunKActorScenarioMode Mode;
	
	UPROPERTY( replicated )
	TArray<AActor*> AcquiredActors;
};


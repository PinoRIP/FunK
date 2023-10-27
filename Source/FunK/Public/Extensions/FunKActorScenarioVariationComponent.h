// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRootVariationComponent.h"
#include "Components/ActorComponent.h"
#include "FunKTestFragment.h"
#include "Util/FunKUtilTypes.h"
#include "FunKActorScenarioVariationComponent.generated.h"

class UFunKActorScenarioVariationComponent;
class UFunKSceneActorResetHandler;

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

	UPROPERTY(EditAnywhere, meta=(EditCondition="Class"))
	FTransform Transform;

	operator bool() const
	{
		return !!Class;
	}
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationSceneActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	AActor* Actor = nullptr;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Actor"))
	TSubclassOf<UFunKSceneActorResetHandler> ResetHandlerClass;

	operator bool() const
	{
		return !!Actor;
	}
};

USTRUCT(BlueprintType)
struct FFunKActorScenarioVariationActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EFunKActorScenarioVariationOwnership Ownership = EFunKActorScenarioVariationOwnership::None;
	
	UPROPERTY(EditAnywhere)
	FFunKActorScenarioVariationSceneActor SceneActor;

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

// Just a cheap wrapper for the old concept for now...

UCLASS(NotBlueprintType, HideDropdown)
class UFunKActorScenarioVariationFragment : public UFunKTestFragment
{
	GENERATED_BODY()

public:
	virtual void OnAdded() override;
	virtual void OnRemoved() override;
	virtual FString GetReadableIdent() const override;

private:
	UPROPERTY()
	TWeakObjectPtr<UFunKActorScenarioVariationComponent> Spawner;
	int32 Index = 0;

	friend UFunKActorScenarioVariationComponent;
};

/*
 * Variation component that handles actors.
 */
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
	virtual UFunKTestFragment* GetFragment(int32 Index) override;
	virtual bool IsReady(UFunKTestFragment* Instance, int32 Index) override;

private:
	virtual void Begin(int32 index);
	virtual bool IsReady();
	virtual void Finish();
	virtual FString GetName() const;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

protected:
	virtual AActor* AcquireActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* GetSceneActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual AActor* SpawnActor(const FFunKActorScenarioVariationActor& VariationActor);
	virtual void AssignOwner(AActor* Actor, EFunKActorScenarioVariationOwnership Ownership);
	virtual void ReleaseActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	virtual void ReleaseSceneActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	// virtual void CopyPropertiesFromTo(UObject* From, UObject* To);
	virtual void ReleaseSpawnActor(AActor* Actor, const FFunKActorScenarioVariationActor& VariationActor);
	virtual APlayerController* GetController(EFunKActorScenarioVariationOwnership Ownership);

	EFunKTestEnvironmentType GetEnvironment() const;
	FFunKOwnershipDistribution GetOwnershipDistribution();
	
private:
	EFunKActorScenarioMode Mode;
	
	UPROPERTY( replicated, Transient )
	TArray<AActor*> AcquiredActors;

	UPROPERTY(Transient)
	TArray<AActor*> InitialActorStates;

	UPROPERTY(Transient)
	TMap<AActor*, UFunKSceneActorResetHandler*> ResetHandlers;
	
public:
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetActor(int32 InIndex = 0);

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetActorByOwnership(EFunKActorScenarioVariationOwnership Ownership, int32 InIndex = 0);

	UFUNCTION(BlueprintCallable)
	EFunKActorScenarioVariationOwnership GetLocalOwnerships();

	UFUNCTION(BlueprintCallable)
	bool IsAppositionPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsOppositionPlayer();

	UFUNCTION(BlueprintCallable)
	bool IsAI();

private:
	friend UFunKActorScenarioVariationFragment;
};


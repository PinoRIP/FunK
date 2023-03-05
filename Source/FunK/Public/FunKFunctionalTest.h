// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKFunctionalTestResult.h"
#include "GameFramework/Actor.h"
#include "FunkFunctionalTest.generated.h"

USTRUCT(BlueprintType)
struct FFunKTimeLimit
{
	GENERATED_BODY()

	/** Test's time limit. '0' means no limit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Time;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFunKFunctionalTestResult Result = EFunKFunctionalTestResult::Failed;
};


UCLASS()
class FUNK_API AFunKFunctionalTest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunKFunctionalTest();
	
	/**
	 * A description of the test, like what is this test trying to determine.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Funk", meta = (MultiLine = "true"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk")
	bool IsEnabled = true;

	/** The Test's time limit for preparation, this is the time it has to return true when checking IsReady(). '0' means no limit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup|Timeout")
	FFunKTimeLimit PreparationTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup|Timeout")
	FFunKTimeLimit NetworkingTimeLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup|Timeout")
	FFunKTimeLimit TimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup")
	bool RunInStandaloneMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup")
	bool RunInDedicatedServerMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup")
	bool RunInListenServerMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup")
	bool RunOnServer = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Funk|Setup")
	bool RunOnClient = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BuildTestRegistry(FString& append) const;
	
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif // WITH_EDITOR
};

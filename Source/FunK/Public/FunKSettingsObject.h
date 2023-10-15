// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Setup/FunKTimeLimit.h"
#include "FunKSettingsObject.generated.h"

class UFunKTestRunner;
class AFunKWorldTestController;

UENUM(BlueprintType)
enum class EFunKTestDiscoveryMethod : uint8
{
	/*Searches through every map*/
	Search,
	/*Searches through maps that start with a defined prefix*/
	Prefix,
	/*Searches through specifically configured worlds*/
	Worlds,
	/*Searches through all worlds that match the asset paths*/
	Paths
};

USTRUCT(meta=(ShowOnlyInnerProperties))
struct FFunKSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = Global)
	TSubclassOf<AFunKWorldTestController> WorldTestControllerClassOverride;
	
	UPROPERTY(config, EditAnywhere, Category = Global)
	TSubclassOf<UFunKTestRunner> TestRunnerClassOverride;
	
	UPROPERTY(config, EditAnywhere, Category = Global)
	bool RunTestUnderOneProcess = true;

	UPROPERTY(config, EditAnywhere, Category = FixedTick)
	TArray<int32> FixedTickFrameRates;
	
	UPROPERTY(config, EditAnywhere, Category = Synchronization)
	FFunKTimeLimit SyncTimeLimit;
	
	UPROPERTY(config, EditAnywhere, Category = "Test discovery")
	EFunKTestDiscoveryMethod DiscoveryMethod = EFunKTestDiscoveryMethod::Prefix;
	
	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Prefix", EditConditionHides))
	FString Prefix = FString("FUNK_");

	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Worlds", EditConditionHides))
	TArray<TSoftObjectPtr<UWorld>> Worlds;
	
	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Paths", EditConditionHides))
	TArray<FString> Paths;
};

/**
 * 
 */
UCLASS(config=FunK, defaultconfig, meta=(DisplayName="FunK"))
class FUNK_API UFunKSettingsObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "FunK", meta=(ShowOnlyInnerProperties))
	FFunKSettings Settings;
};

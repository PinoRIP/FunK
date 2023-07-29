// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stages/FunKTimeLimit.h"
#include "FunKSettingsObject.generated.h"

class UFunKTestRunner;
class AFunKWorldTestController;

UENUM(BlueprintType)
enum class EFunKTestDiscoveryMethod : uint8
{
	Search,
	Prefix,
	Worlds,
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

	UPROPERTY(config, EditAnywhere, Category = FixedTick)
	TArray<int32> FixedTickFrameRates;
	
	UPROPERTY(config, EditAnywhere, Category = Synchronization)
	FFunKTimeLimit SyncTimeLimit;
	
	UPROPERTY(config, EditAnywhere, Category = "Test discovery")
	EFunKTestDiscoveryMethod DiscoveryMethod;
	
	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Prefix", EditConditionHides))
	FString Prefix = FString("FUNK_");

	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Worlds", EditConditionHides))
	TArray<TAssetPtr<UWorld>> Worlds;
	
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

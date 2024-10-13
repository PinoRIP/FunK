// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Setup/FunKTimeLimit.h"
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
	// World test controller spawned when there is no existing controller
	UPROPERTY(config, EditAnywhere, Category = Global)
	TSubclassOf<AFunKWorldTestController> WorldTestControllerClassOverride;

	// Test runner used
	// UPROPERTY(config, EditAnywhere, Category = Global)
	// TSubclassOf<UFunKTestRunner> TestRunnerClassOverride;

	// Run under one process (Is more performant but less "realistic"?)
	UPROPERTY(config, EditAnywhere, Category = Global)
	bool RunTestUnderOneProcess = true;

	/* With this the Frame rate can be fixed:
	 *	0 Entry: all instances tick freely
	 *	1 Entry: all instances tick with the same framerate
	 *	2 Entries: The Server ticks with entry 0, the clients with entry 1
	 *	3 Entries: The Server ticks with 0, client 1 with 1, client 2 with 2
	 *  >3 Entries: Irrelevant
	 */
	UPROPERTY(config, EditAnywhere, Category = FixedTick)
	TArray<int32> FixedTickFrameRates;

	// Timeout for synchronizing peers between stages
	UPROPERTY(config, EditAnywhere, Category = Synchronization)
	FFunKTimeLimit SyncTimeLimit;

	// Method of discovery for tests
	UPROPERTY(config, EditAnywhere, Category = "Test discovery")
	EFunKTestDiscoveryMethod DiscoveryMethod = EFunKTestDiscoveryMethod::Prefix;

	// World prefix
	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Prefix", EditConditionHides))
	FString Prefix = FString("FUNK_");

	// World reference
	UPROPERTY(config, EditAnywhere, Category = "Test discovery", meta=(EditCondition = "DiscoveryMethod == EFunKTestDiscoveryMethod::Worlds", EditConditionHides))
	TArray<TSoftObjectPtr<UWorld>> Worlds;

	// Paths to search through
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

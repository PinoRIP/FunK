// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stages/FunKTimeLimit.h"
#include "FunKSettingsObject.generated.h"

class UFunKTestRunner;
class AFunKWorldTestController;

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

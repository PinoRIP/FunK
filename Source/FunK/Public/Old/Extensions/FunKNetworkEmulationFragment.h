// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Extensions/FunKTestFragment.h"
#include "Old/Util/FunKNetworkEmulationTypes.h"
#include "Engine/NetDriver.h"
#include "FunKNetworkEmulationFragment.generated.h"

/**
 * Fragment that changes the net driver settings to emulate networking quality. After the fragment is removed the settings will be restored.
 */
UCLASS(NotBlueprintType)
class UFunKNetworkEmulationFragment : public UFunKTestFragment
{
	GENERATED_BODY()

public:
	virtual void OnAdded() override;
	virtual void OnRemoved() override;
	virtual FString GetLogName() const override;
	
	UPROPERTY()
	FFunKNetworkEmulation NetworkEmulation;

private:
	TArray<TKeyValuePair<TWeakObjectPtr<UNetDriver>, FPacketSimulationSettings>> InitialDriverSettings;
};

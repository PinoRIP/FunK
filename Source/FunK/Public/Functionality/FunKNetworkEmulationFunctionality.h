// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestFunctionality.h"
#include "Util/FunKNetworkEmulationTypes.h"
#include "Engine/NetDriver.h"
#include "FunKNetworkEmulationFunctionality.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintType)
class UFunKNetworkEmulationFunctionality : public UFunKTestFunctionality
{
	GENERATED_BODY()

public:
	virtual void OnAdded() override;
	virtual void OnRemoved() override;
	virtual FString GetReadableIdent() const override;
	
	UPROPERTY()
	FFunKNetworkEmulation NetworkEmulation;

private:
	TArray<TKeyValuePair<TWeakObjectPtr<UNetDriver>, FPacketSimulationSettings>> InitialDriverSettings;
};

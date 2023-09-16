// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKInputSimulationBlueprintFunctionLibrary.generated.h"

struct FFunKInputSimulationHandle;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKInputSimulationBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulationHandle", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndInputSimulation(UObject* WorldContext, const FFunKInputSimulationHandle& Handle);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|InputSimulationHandle", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static void EndInputSimulations(UObject* WorldContext, const TArray<FFunKInputSimulationHandle>& Handles);
};

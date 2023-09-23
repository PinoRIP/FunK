// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKUtilTypes.h"
#include "FunKWorldSubsystem.h"
#include "FunKUtilBlueprintFunctionLibrary.generated.h"



/**
 * 
 */
UCLASS()
class FUNK_API UFunKBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static UFunKWorldSubsystem* GetFunKWorldSubsystem(UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static int32 GetPeerIndex(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetMode GetNetMode(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetMode(UObject* WorldContext, EFunKNetMode& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKClient GetClients(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchClients(UObject* WorldContext, EFunKClient& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKTestEnvironmentType GetTestEnvironmentType(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchTestEnvironmentType(UObject* WorldContext, EFunKTestEnvironmentType& Branches);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static bool IsServerDedicated(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetLocation GetNetLocation(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetLocation(UObject* WorldContext, EFunKNetLocation& Branches);
};

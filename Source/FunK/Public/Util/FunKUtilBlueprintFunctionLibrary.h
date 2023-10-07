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
	static UFunKWorldSubsystem* GetFunKWorldSubsystem(const UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static int32 GetPeerIndex(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetMode GetNetMode(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetMode(const UObject* WorldContext, EFunKNetMode& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKClient GetClients(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchClients(const UObject* WorldContext, EFunKClient& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKTestEnvironmentType GetTestEnvironmentType(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchTestEnvironmentType(const UObject* WorldContext, EFunKTestEnvironmentType& Branches);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static bool IsServerDedicated(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetLocation GetNetLocation(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetLocation(const UObject* WorldContext, EFunKNetLocation& Branches);
};

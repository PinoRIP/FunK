// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKConsoleBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FUNK_API UFunKConsoleBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static FString GetConsoleVariable(const FString& Name, bool& IsValid);

	static FString GetConsoleVariable(const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static FString SetConsoleVariable(const FString& Name, const FString& Value, bool& IsValid);

	static FString SetConsoleVariable(const FString& Name, const FString& Value);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static float GetConsoleVariableFloat(const FString& Name, bool& IsValid);
	
	static float GetConsoleVariableFloat(const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static float SetConsoleVariableFloat(const FString& Name, float Value, bool& IsValid);
	
	static float SetConsoleVariableFloat(const FString& Name, float Value);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static bool GetConsoleVariableBool(const FString& Name, bool& IsValid);
	
	static bool GetConsoleVariableBool(const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static bool SetConsoleVariableBool(const FString& Name, bool Value, bool& IsValid);
	
	static bool SetConsoleVariableBool(const FString& Name, bool Value);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static int32 GetConsoleVariableInt(const FString& Name, bool& IsValid);
	
	static int32 GetConsoleVariableInt(const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Console")
	static int32 SetConsoleVariableInt(const FString& Name, int32 Value, bool& IsValid);
	
	static int32 SetConsoleVariableInt(const FString& Name, int32 Value);

private:
	static IConsoleVariable* GetVariable(const FString& Name, bool& IsValid);
};

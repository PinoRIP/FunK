// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Extensions/FunKTestFragment.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKFragmentBlueprintFunctionLibrary.generated.h"

class UFunKTestVariationComponent;
class UFunKTestRootVariationComponent;
class AFunKTestBase;
/**
 * 
 */
UCLASS()
class FUNK_API UFunKFragmentBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Fragments", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* MakeTestFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Fragments", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* MakeStageFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestRootVariationComponent* GetRootVariationComponent(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static int32 GetRootVariationIndex(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* GetRootVariationFragment(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool HasRootVariation(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestVariationComponent* GetVariationComponent(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static int32 GetVariationIndex(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* GetVariationFragment(AFunKTestBase* Context);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FunK", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static bool HasVariation(AFunKTestBase* Context);
	
};

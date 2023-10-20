// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Extensions/FunKTestFragment.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunKTestFragmentBlueprintFunctionLibrary.generated.h"

class AFunKTestBase;
/**
 * 
 */
UCLASS()
class FUNK_API UFunKTestFragmentBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Fragments", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* MakeTestFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Fragments", meta = ( HidePin = "Context", DefaultToSelf = "Context"))
	static UFunKTestFragment* MakeStageFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class);
	
};

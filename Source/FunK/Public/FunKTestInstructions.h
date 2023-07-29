// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestInstructions.generated.h"

/**
 * 
 */
USTRUCT()
struct FFunKTestInstructions
{
	GENERATED_BODY()

	FFunKTestInstructions()
		: FFunKTestInstructions(FString(), FString(), FString(), FString())
	{ }

	FFunKTestInstructions(FString mapObjectPath, FString mapPackageName, FString mapTestName, FString params)
	{
		MapObjectPath = mapObjectPath;
		MapPackageName = mapPackageName;
		MapTestName = mapTestName;
		Params = params;
	}
	
	FString MapObjectPath;
	FString MapPackageName;
	FString MapTestName;
	FString Params;

	bool IsStandaloneTest() const;
	bool IsDedicatedServerTest() const;
	bool IsListenServerTest() const;
};

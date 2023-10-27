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

	FFunKTestInstructions(FString InMapObjectPath, FString InMapPackageName, FString InMapTestName, FString InParams)
	{
		MapObjectPath = InMapObjectPath;
		MapPackageName = InMapPackageName;
		MapTestName = InMapTestName;
		Params = InParams;
	}
	
	FString MapObjectPath;
	FString MapPackageName;
	FString MapTestName;
	FString Params;

	bool IsStandaloneTest() const;
	bool IsDedicatedServerTest() const;
	bool IsListenServerTest() const;
};

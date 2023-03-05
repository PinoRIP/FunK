// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "UObject/Object.h"

/**
 * 
 */
class FUNK_API FFunKAutomationEntry : public FAutomationTestBase
{
public:
	FFunKAutomationEntry(const FString& InName, bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask)
	{
	}

	static void ParseTestMapInfo(const FString& Parameters, FString& MapObjectPath, FString& MapPackageName, FString& MapTestName, FString& Params);
};

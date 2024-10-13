// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Context values for test lifetime events
class FunKTestLifeTimeContext
{
public:
	static FString BeginTest;
	static FString BeginStage;
	static FString FinishStage;
	static FString FinishTest;
};

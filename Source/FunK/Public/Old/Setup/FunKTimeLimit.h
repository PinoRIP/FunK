// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/FunKTestResult.h"
#include "FunKTimeLimit.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FUNK_API FFunKTimeLimit
{
	GENERATED_BODY()

	/** Test's time limit. '0' means no limit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Time = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFunKTestResult Result = EFunKTestResult::Failed;

	bool IsTimeout(float MsPassed) const;

	bool IsLimitless() const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKFunctionalTestResult.h"
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
	float Time = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFunKFunctionalTestResult Result = EFunKFunctionalTestResult::Failed;

	bool IsTimeout(float time) const;

	bool IsLimitless() const;
};

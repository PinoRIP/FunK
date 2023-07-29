﻿#pragma once


UENUM(BlueprintType)
enum class EFunKTestResult : uint8
{
	None,
	Error,
	Failed,
	Invalid,
	Skipped,
	Succeeded,
};

/* Return a readable string of the provided EFunctionalTestResult enum */
FString FUNK_API LexToString(const EFunKTestResult TestResult);

UENUM(BlueprintType)
enum class EFunKStageResult : uint8
{
	None UMETA(Hidden),
	Error,
	Failed,
	Succeeded,
};

/* Return a readable string of the provided EFunctionalTestResult enum */
FString FUNK_API LexToString(const EFunKStageResult TestResult);

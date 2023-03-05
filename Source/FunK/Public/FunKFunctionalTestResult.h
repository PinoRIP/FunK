#pragma once


UENUM(BlueprintType)
enum class EFunKFunctionalTestResult : uint8
{
	None,
	Default,
	Invalid,
	Error,
	Failed,
	Succeeded,
	Skipped
};

/* Return a readable string of the provided EFunctionalTestResult enum */
FString FUNK_API LexToString(const EFunKFunctionalTestResult TestResult);

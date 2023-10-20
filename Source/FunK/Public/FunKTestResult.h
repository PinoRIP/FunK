#pragma once


/* Result of a test */
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

/* Result of a stage */
UENUM(BlueprintType)
enum class EFunKStageResult : uint8
{
	Error,
	Failed,
	Skipped,
	Succeeded,
	None UMETA(Hidden),
};

/* Return a readable string of the provided EFunctionalTestResult enum */
FString FUNK_API LexToString(const EFunKStageResult TestResult);

EFunKTestResult FUNK_API StageToTestResult(const EFunKStageResult TestResult);
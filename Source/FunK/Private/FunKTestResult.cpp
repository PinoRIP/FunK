#include "FunKTestResult.h"

FString LexToString(const EFunKTestResult TestResult)
{
	switch (TestResult) {
	case EFunKTestResult::None: return FString("None");
	case EFunKTestResult::Invalid: return FString("Invalid");
	case EFunKTestResult::Error: return FString("Error");
	case EFunKTestResult::Failed: return FString("Failed");
	case EFunKTestResult::Succeeded: return FString("Succeeded");
	case EFunKTestResult::Skipped: return FString("Skipped");
	default: return FString("Unhandled EFunKTestResult Enum!");
	}
}

FString LexToString(const EFunKStageResult TestResult)
{
	switch (TestResult) {
	case EFunKStageResult::None: return FString("None");
	case EFunKStageResult::Error: return FString("Error");
	case EFunKStageResult::Failed: return FString("Failed");
	case EFunKStageResult::Succeeded: return FString("Succeeded");
	default: return FString("Unhandled EFunKStageResult Enum!");
	}
}

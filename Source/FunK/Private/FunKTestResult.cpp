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
	default: return FString("Unhandled EFunKFunctionalTestResult Enum!");
	}
}

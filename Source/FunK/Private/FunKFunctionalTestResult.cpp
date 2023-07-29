#include "FunKFunctionalTestResult.h"

FString LexToString(const EFunKFunctionalTestResult TestResult)
{
	switch (TestResult) {
	case EFunKFunctionalTestResult::None: return FString("None");
	case EFunKFunctionalTestResult::Default: return FString("Default");
	case EFunKFunctionalTestResult::Invalid: return FString("Invalid");
	case EFunKFunctionalTestResult::Error: return FString("Error");
	case EFunKFunctionalTestResult::Failed: return FString("Failed");
	case EFunKFunctionalTestResult::Succeeded: return FString("Succeeded");
	case EFunKFunctionalTestResult::Skipped: return FString("Skipped");
	default: return FString("Unhandled EFunKFunctionalTestResult Enum!");
	}
}

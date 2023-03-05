#pragma once

UENUM(BlueprintType)
enum class EFunKFunctionalTestResult : uint8
{
	/**
	 * When finishing a test if you use Default, you're not explicitly stating if the test passed or failed.
	 * Instead you're instead allowing any tested assertions to have decided that for you.  Even if you do
	 * explicitly log success, it can be overturned by errors that occurred during the test.
	 */
	Default,
	Invalid,
	Error,
	Running,
	Failed,
	Succeeded,
	Skipped
};

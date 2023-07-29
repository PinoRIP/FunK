#pragma once

UENUM(BlueprintType)
enum class EFunKTestRunnerType : uint8
{
	None,
	LocalInProc,
	LocalExt,
	RemoteExt
};

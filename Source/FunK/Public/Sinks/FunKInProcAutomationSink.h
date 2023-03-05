// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKSink.h"
#include "UObject/Object.h"
#include "FunKInProcAutomationSink.generated.h"

class FFunKAutomationEntryRuntime;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKInProcAutomationSink : public UFunKSink
{
	GENERATED_BODY()

public:
	virtual void Init(UFunKTestRunner* run) override;
	virtual void RaiseEvent(const FFunKEvent& raisedEvent, const UFunKTestRunner* run) override;
	static bool IsAvailable();

private:
	FFunKAutomationEntry* AutomationEntryRuntime;

	static EAutomationEventType GetAutomationEventType(EFunKEventType eventType);
	static FFunKAutomationEntry* GetCurrentAutomationEntry();
};

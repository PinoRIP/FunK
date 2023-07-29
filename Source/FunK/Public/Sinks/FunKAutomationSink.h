// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKSink.h"
#include "UObject/Object.h"
#include "FunKEvent.h"
#include "FunKAutomationSink.generated.h"

class FFunKAutomationEntry;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKAutomationSink : public UObject, public IFunKSink
{
	GENERATED_BODY()

public:
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	static bool IsAvailable();
	
private:
	static EAutomationEventType GetAutomationEventType(EFunKEventType eventType);
	static FFunKAutomationEntry* GetCurrentAutomationEntry();
};

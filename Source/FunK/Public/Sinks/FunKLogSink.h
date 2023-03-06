// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKSink.h"
#include "UObject/Object.h"
#include "FunKLogSink.generated.h"

/**
 * 
 */
UCLASS()
class FUNK_API UFunKLogSink : public UObject, public IFunKSink
{
	GENERATED_BODY()

public:
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const override;

	static void LogEvent(const FFunKEvent& raisedEvent);
};

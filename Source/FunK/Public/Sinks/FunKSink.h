// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEvent.h"
#include "FunKSink.generated.h"

class UFunKTestRunner;


// This class does not need to be modified.
UINTERFACE(BlueprintType)
class FUNK_API UFunKSink : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FUNK_API IFunKSink
{
	GENERATED_BODY()

public:
	virtual void RaiseEvent(const FFunKEvent& raisedEvent) const = 0;
};

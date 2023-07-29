// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEvent.h"
#include "UObject/Object.h"
#include "FunKSink.generated.h"

class UFunKTestRunner;

/**
 * 
 */
UCLASS()
class FUNK_API UFunKSink : public UObject
{
	GENERATED_BODY()

public:
	virtual void Init(UFunKTestRunner* run) { }
	virtual void RaiseEvent(const FFunKEvent& raisedEvent, const UFunKTestRunner* run) { }
	virtual void End(UFunKTestRunner* run) { }
};

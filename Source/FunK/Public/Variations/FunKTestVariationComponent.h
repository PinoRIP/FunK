// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FunKTestVariationComponent.generated.h"


class AFunKTestBase;

UCLASS(ClassGroup=(Custom), Abstract)
class FUNK_API UFunKTestVariationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKTestVariationComponent();

	virtual int32 GetCount();
	virtual void Begin(int32 index);
	virtual bool IsReady();
	virtual void Finish();
	virtual FString GetName();

	int32 GetIndex() const { return Index; }

	AFunKTestBase* GetOwningTest() const;

protected:
	void Error(const FString& Message) const;
	
private:
	int32 Index = INDEX_NONE;
};

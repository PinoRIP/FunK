// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations/FunKTestVariationComponent.h"

#include "FunKLogging.h"
#include "Internal/FunKTestBase.h"


// Sets default values for this component's properties
UFunKTestVariationComponent::UFunKTestVariationComponent()
{
}

int32 UFunKTestVariationComponent::GetCount()
{
	return 0;
}

void UFunKTestVariationComponent::Begin(int32 index)
{
	Index = index;
}

bool UFunKTestVariationComponent::IsReady()
{
	return true;
}

void UFunKTestVariationComponent::Finish()
{
	Index = INDEX_NONE;
}

FString UFunKTestVariationComponent::GetName()
{
	return FString::FromInt(Index);
}

AFunKTestBase* UFunKTestVariationComponent::GetOwningTest() const
{
	return Cast<AFunKTestBase>(GetOwner());
}

void UFunKTestVariationComponent::Error(const FString& Message) const
{
	if(const AFunKTestBase* Test = GetOwningTest())
	{
		Test->RaiseEvent(FFunKEvent::Error(Message).Ref());
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("%s"), *Message)
	}
}


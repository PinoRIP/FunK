// Fill out your copyright notice in the Description page of Project Settings.


#include "Extensions/FunKTestVariationComponent.h"


FString UFunKTestVariationNoFragment::GetReadableIdent() const
{
	return "No fragment";
}

// Sets default values for this component's properties
UFunKTestVariationComponent::UFunKTestVariationComponent()
{
}

int32 UFunKTestVariationComponent::GetCount()
{
	return 0;
}

UFunKTestFragment* UFunKTestVariationComponent::GetFragment(int32 Index)
{
	return NewObject<UFunKTestVariationNoFragment>();
}

bool UFunKTestVariationComponent::IsReady(UFunKTestFragment* Instance, int32 Index)
{
	return Instance != nullptr;
}

void UFunKTestVariationComponent::OnUsing(UFunKTestFragment* Instance)
{
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Extensions/FunKTestVariationComponent.h"


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
	check(false)
	return nullptr;
}

bool UFunKTestVariationComponent::IsReady(UFunKTestFragment* Instance, int32 Index)
{
	return Instance != nullptr;
}

void UFunKTestVariationComponent::OnUsing(UFunKTestFragment* Instance)
{
}

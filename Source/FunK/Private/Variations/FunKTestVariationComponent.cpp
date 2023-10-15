// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations/FunKTestVariationComponent.h"


// Sets default values for this component's properties
UFunKTestVariationComponent::UFunKTestVariationComponent()
{
}

int32 UFunKTestVariationComponent::GetCount()
{
	return 0;
}

UFunKTestFunctionality* UFunKTestVariationComponent::GetFunctionality(int32 Index)
{
	check(false)
	return nullptr;
}

bool UFunKTestVariationComponent::IsReady(UFunKTestFunctionality* Instance, int32 Index)
{
	return Instance != nullptr;
}

void UFunKTestVariationComponent::OnUsing(UFunKTestFunctionality* Instance)
{
}

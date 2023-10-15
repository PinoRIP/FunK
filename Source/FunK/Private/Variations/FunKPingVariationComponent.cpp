// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations/FunKNetworkEmulationVariationComponent.h"
#include "Engine/NetDriver.h"
#include "Functionality/FunKNetworkEmulationFunctionality.h"


// Sets default values for this component's properties
UFunKNetworkEmulationVariationComponent::UFunKNetworkEmulationVariationComponent()
{
}

int32 UFunKNetworkEmulationVariationComponent::GetCount()
{
#if DO_ENABLE_NET_TEST
	return Emulations.Num();
#else
	return 0;
#endif
}

UFunKTestFunctionality* UFunKNetworkEmulationVariationComponent::GetFunctionality(int32 Index)
{
	UFunKNetworkEmulationFunctionality* Functionality = NewObject<UFunKNetworkEmulationFunctionality>(this);
	Functionality->NetworkEmulation = Emulations[Index];
	return Functionality;
}




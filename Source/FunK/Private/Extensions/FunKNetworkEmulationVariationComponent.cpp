// Fill out your copyright notice in the Description page of Project Settings.


#include "Extensions/FunKNetworkEmulationVariationComponent.h"
#include "Engine/NetDriver.h"
#include "Extensions/FunKNetworkEmulationFragment.h"


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

UFunKTestFragment* UFunKNetworkEmulationVariationComponent::GetFragment(int32 Index)
{
	UFunKNetworkEmulationFragment* Fragment = NewObject<UFunKNetworkEmulationFragment>(this);
	Fragment->NetworkEmulation = Emulations[Index];
	return Fragment;
}




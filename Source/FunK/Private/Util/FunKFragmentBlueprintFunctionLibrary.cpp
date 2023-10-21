// Fill out your copyright notice in the Description page of Project Settings.

#include "Util/FunKFragmentBlueprintFunctionLibrary.h"
#include "FunKTestBase.h"

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::MakeTestFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	if (!Context) return nullptr;
	UFunKTestFragment* Fragment = Context->AddTestFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::MakeStageFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	if (!Context) return nullptr;
	UFunKTestFragment* Fragment = Context->AddStageFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

UFunKTestRootVariationComponent* UFunKFragmentBlueprintFunctionLibrary::GetRootVariationComponent(AFunKTestBase* Context)
{
	if (!Context) return nullptr;
	return Context->GetCurrentVariation().Root;
}

int32 UFunKFragmentBlueprintFunctionLibrary::GetRootVariationIndex(AFunKTestBase* Context)
{
	if (!Context) return INDEX_NONE;
	return Context->GetCurrentVariation().RootIndex;
}

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::GetRootVariationFragment(AFunKTestBase* Context)
{
	if (!Context) return nullptr;
	return Context->GetCurrentVariation().RootFragment;
}

bool UFunKFragmentBlueprintFunctionLibrary::HasRootVariation(AFunKTestBase* Context)
{
	return GetRootVariationComponent(Context) != nullptr;
}

UFunKTestVariationComponent* UFunKFragmentBlueprintFunctionLibrary::GetVariationComponent(AFunKTestBase* Context)
{
	if (!Context) return nullptr;
	return Context->GetCurrentVariation().Variation;
}

int32 UFunKFragmentBlueprintFunctionLibrary::GetVariationIndex(AFunKTestBase* Context)
{
	if (!Context) return INDEX_NONE;
	return Context->GetCurrentVariation().Index;
}

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::GetVariationFragment(AFunKTestBase* Context)
{
	if (!Context) return nullptr;
	return Context->GetCurrentVariation().Fragment;
}

bool UFunKFragmentBlueprintFunctionLibrary::HasVariation(AFunKTestBase* Context)
{
	return GetVariationComponent(Context) != nullptr;
}

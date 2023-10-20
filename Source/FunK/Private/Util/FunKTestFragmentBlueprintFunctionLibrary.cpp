// Fill out your copyright notice in the Description page of Project Settings.

#include "Util/FunKTestFragmentBlueprintFunctionLibrary.h"
#include "FunKLogging.h"
#include "FunKTestBase.h"

UFunKTestFragment* UFunKTestFragmentBlueprintFunctionLibrary::MakeTestFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	UFunKTestFragment* Fragment = Context->AddTestFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

UFunKTestFragment* UFunKTestFragmentBlueprintFunctionLibrary::MakeStageFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	UFunKTestFragment* Fragment = Context->AddStageFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

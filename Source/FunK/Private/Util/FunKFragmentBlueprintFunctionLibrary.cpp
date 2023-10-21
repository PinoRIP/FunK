// Fill out your copyright notice in the Description page of Project Settings.

#include "Util/FunKFragmentBlueprintFunctionLibrary.h"
#include "FunKLogging.h"
#include "FunKTestBase.h"

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::MakeTestFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	UFunKTestFragment* Fragment = Context->AddTestFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

UFunKTestFragment* UFunKFragmentBlueprintFunctionLibrary::MakeStageFragment(AFunKTestBase* Context, TSubclassOf<UFunKTestFragment> Class)
{
	UFunKTestFragment* Fragment = Context->AddStageFragment(NewObject<UFunKTestFragment>(Context, Class));
	return Fragment;
}

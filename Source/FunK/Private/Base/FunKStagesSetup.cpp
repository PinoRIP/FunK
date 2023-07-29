// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/FunKStagesSetup.h"

int32 FFunKStageSetupBase::GetStageIndex() const
{
	if(!Stages)
		return INDEX_NONE;
	
	const int32 count = Stages->Stages.Num();
	for(int32 i = 0; count > i; i++)
	{
		if(&Stages->Stages[i] == Stage)
			return i;
	}
	return INDEX_NONE;
}

void FFunKStageSetupBase::Update(int32& count, bool& oldValue, bool newValue)
{
	if(!newValue && oldValue)
		count--;
	else if(!oldValue)
		count++;

	oldValue = newValue;
}

// ReSharper disable once CppMemberFunctionMayBeConst - This being const makes resharper warn the caller when the return ist not used.
FFunKLatentStageSetup FFunKStageSetup::MakeLatent()
{
	if(Stage)
		Stage->IsLatent = true;
	
	return FFunKLatentStageSetup(Stage, Stages, TestBase);
}

int32 FFunKStagesSetup::Num() const
{
	if(Stages)
		return Stages->Stages.Num();

	return 0;
}

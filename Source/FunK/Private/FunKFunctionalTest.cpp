﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"
#include "Setup/FunKStagesSetup.h"


AFunKFunctionalTest::AFunKFunctionalTest()
	: AFunKTestBase()
{
	ArrangeTimeLimit.Message = FText::FromString("Preparation time limit reached");
	ActTimeLimit.Message = FText::FromString("Execution time limit reached");
	AssertTimeLimit.Message = FText::FromString("Assertion time limit reached");
}

void AFunKFunctionalTest::SetupStages(FFunKStagesSetup& StageSetup)
{
	SetupFunctionalTestStages(StageSetup);

	auto Iterator = StageSetup.GetStageSetupBaseIterator();
	while (Iterator.Next())
	{
		Iterator.Get()
			.SetRunOnStandalone(RunOnStandalone)
			.SetRunOnListenServer(RunOnListenServer)
			.SetRunOnListenServerClient(RunOnListenServerClients)
			.SetRunOnDedicatedServer(RunOnDedicatedServer)
			.SetRunOnDedicatedServerClient(RunOnDedicatedServerClients);
	}
}

void AFunKFunctionalTest::SetupFunctionalTestStages(FFunKStagesSetup& StageSetup)
{
	StageSetup
		.AddNamedStage<AFunKFunctionalTest>("Assume", &AFunKFunctionalTest::InvokeAssume);

	Super::SetupStages(StageSetup);
	
	StageSetup.AddNamedLatentStage<AFunKFunctionalTest>("Arrange", &AFunKFunctionalTest::InvokeArrange)
		.UpdateTimeLimit(ArrangeTimeLimit)
		.WithOptionalBpTickDelegate(AFunKFunctionalTest, BpArrangeTick)
	
		.ThenAddLatentStage(AFunKFunctionalTest, Act)
		.UpdateTimeLimit(ActTimeLimit)
		.WithOptionalBpTickDelegate(AFunKFunctionalTest, BpActTick);

	if (LatentAssert)
	{
		StageSetup.AddNamedLatentStage<AFunKFunctionalTest>("Assert", &AFunKFunctionalTest::InvokeAssert)
			.UpdateTimeLimit(AssertTimeLimit)
			.WithOptionalBpTickDelegate(AFunKFunctionalTest, BpAssertTick);
	}
	else
	{
		StageSetup.AddNamedStage<AFunKFunctionalTest>("Assert", &AFunKFunctionalTest::InvokeAssert);
	}
}

void AFunKFunctionalTest::InvokeAssume()
{
	if (!Assume())
	{
		FinishStage(EFunKStageResult::Skipped, "Assumption not met");
	}
}

void AFunKFunctionalTest::InvokeArrange()
{
	if (Arrange())
	{
		FinishStage();
	}
}

void AFunKFunctionalTest::InvokeAssert()
{
	if (Assert() || !LatentAssert)
	{
		FinishStage();
	}
}

bool AFunKFunctionalTest::Assume()
{
	return BpAssume();
}

bool AFunKFunctionalTest::BpAssume_Implementation()
{
	return true;
}

bool AFunKFunctionalTest::Arrange()
{
	if (IsBpEventImplemented(GET_FUNCTION_NAME_CHECKED(AFunKFunctionalTest, BpArrange)))
	{
		BpArrange();
		return false;
	}
	
	return true;
}

void AFunKFunctionalTest::BpArrange_Implementation()
{
}

void AFunKFunctionalTest::BpArrangeTick_Implementation(float DeltaTime)
{
}

void AFunKFunctionalTest::Act()
{
	BpAct();
}

void AFunKFunctionalTest::BpActTick_Implementation(float DeltaTime)
{
}

bool AFunKFunctionalTest::Assert()
{
	if (IsBpEventImplemented(GET_FUNCTION_NAME_CHECKED(AFunKFunctionalTest, BpAssert)))
	{
		BpAssert();
		return false;
	}
	
	return true;
}

void AFunKFunctionalTest::BpAssert_Implementation()
{
}

void AFunKFunctionalTest::BpAssertTick_Implementation(float DeltaTime)
{
}

void AFunKFunctionalTest::OnFinish(const FString& Message)
{
	BpCleanup();
	Super::OnFinish(Message);
}

void AFunKFunctionalTest::BpCleanup_Implementation()
{
}

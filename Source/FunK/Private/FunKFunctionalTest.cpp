// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"
#include "Base/FunKStagesSetup.h"


AFunKFunctionalTest::AFunKFunctionalTest()
	: AFunKTestBase()
{
	ArrangeTimeLimit.Message = FText::FromString("Preparation time limit reached");
	ActTimeLimit.Message = FText::FromString("Execution time limit reached");
}

void AFunKFunctionalTest::SetupStages(FFunKStagesSetup& stages)
{
	SetupFunctionalTestStages(stages);

	auto Iterator = stages.GetStageSetupBaseIterator();
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

void AFunKFunctionalTest::SetupFunctionalTestStages(FFunKStagesSetup& stages)
{
	stages
		.AddNamedStage<AFunKFunctionalTest>("Assume", &AFunKFunctionalTest::InvokeAssume)
	
		.ThenAddNamedLatentStage<AFunKFunctionalTest>("Arrange", &AFunKFunctionalTest::InvokeArrange)
		.UpdateTimeLimit(ArrangeTimeLimit)
		.WithOptionalBpTickDelegate(AFunKFunctionalTest, BpArrangeTick)
	
		.ThenAddLatentStage(AFunKFunctionalTest, Act)
		.UpdateTimeLimit(ActTimeLimit)
		.WithOptionalBpTickDelegate(AFunKFunctionalTest, BpActTick)
	
		.ThenAddStage(AFunKFunctionalTest, Assert);
}

void AFunKFunctionalTest::InvokeAssume()
{
	if(!Assume())
	{
		FinishStage(EFunKStageResult::Skipped, "Assumption not met");
	}
}

void AFunKFunctionalTest::InvokeArrange()
{
	if(Arrange())
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
	if(IsBpEventImplemented(GET_FUNCTION_NAME_CHECKED(AFunKFunctionalTest, BpArrange)))
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

void AFunKFunctionalTest::Assert()
{
	BpAssert();
}

void AFunKFunctionalTest::BpAssert_Implementation()
{
}

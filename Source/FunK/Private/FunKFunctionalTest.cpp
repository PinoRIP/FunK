// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"

#include "Stages/FunKStagesSetup.h"


AFunKFunctionalTest::AFunKFunctionalTest()
{
	ArrangeTimeLimit.Message = FText::FromString("Preparation time limit reached");
	ActTimeLimit.Message = FText::FromString("Execution time limit reached");
	SyncTimeLimit.Message = FText::FromString("Sync time limit reached");
}

FFunKTimeLimit* AFunKFunctionalTest::GetSyncTimeLimit()
{
	return &SyncTimeLimit;
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
		.ThenAddNamedLatentStage<AFunKFunctionalTest>("Arrange", &AFunKFunctionalTest::InvokeArrange).UpdateTimeLimit(ArrangeTimeLimit)
		.ThenAddLatentStage(AFunKFunctionalTest, Act).UpdateTimeLimit(ActTimeLimit)
		.ThenAddStage(AFunKFunctionalTest, Assert);
}

void AFunKFunctionalTest::InvokeAssume()
{
	if(!Assume())
	{
		FinishStage(EFunKTestResult::Skipped, "Assumption not met");
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
	return BpArrange();
}

bool AFunKFunctionalTest::BpArrange_Implementation()
{
	return true;
}

void AFunKFunctionalTest::Act()
{
	BpAct();
}

void AFunKFunctionalTest::BpAct_Implementation()
{
}

void AFunKFunctionalTest::Assert()
{
	BpAssert();
}

void AFunKFunctionalTest::BpAssert_Implementation()
{
}

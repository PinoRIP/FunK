// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKEngineSubsystem.h"
#include "FunKTestRunner.h"

UFunKTestRunner* UFunKEngineSubsystem::StartTestRunner()
{
	return SetupTestRun(EFunKTestRunnerType::LocalInProc);
}

UFunKTestRunner* UFunKEngineSubsystem::ConnectTestRunner(bool isRemote)
{
	return SetupTestRun(isRemote ? EFunKTestRunnerType::RemoteExt : EFunKTestRunnerType::LocalExt);
}

UFunKTestRunner* UFunKEngineSubsystem::GetTestRunner()
{
	if(!ActiveTestRun) ActiveTestRun = StartTestRunner();
	return ActiveTestRun;
}

void UFunKEngineSubsystem::EndTestRun()
{
	if(ActiveTestRun && ActiveTestRun->IsRunning())
	{
		ActiveTestRun->End();
		check(!ActiveTestRun->IsRunning())
	}

	ActiveTestRun = nullptr;
}

void UFunKEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UFunKEngineSubsystem::Deinitialize()
{
	Super::Deinitialize();
	EndTestRun();
}

bool UFunKEngineSubsystem::IsRunning() const
{
	return ActiveTestRun && ActiveTestRun->IsRunning();
}

UFunKTestRunner* UFunKEngineSubsystem::SetupTestRun(EFunKTestRunnerType RunType)
{
	EndTestRun();
	
	//TODO: Make this configurable
	ActiveTestRun = NewObject<UFunKTestRunner>(this);
	ActiveTestRun->Init(this, RunType);
	ActiveTestRun->Start();
	check(ActiveTestRun->IsRunning())

	return ActiveTestRun;
}

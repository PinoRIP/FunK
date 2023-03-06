// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKEngineSubsystem.h"
#include "FunK.h"
#include "FunKLogging.h"
#include "FunKTestRunner.h"

UFunKTestRunner* UFunKEngineSubsystem::StartTestRunner()
{
	return SetupTestRun(EFunKTestRunnerType::LocalInProc);
}

UFunKTestRunner* UFunKEngineSubsystem::ConnectTestRunner(bool isRemote)
{
	return SetupTestRun(isRemote ? EFunKTestRunnerType::RemoteExt : EFunKTestRunnerType::LocalExt);
}

UFunKTestRunner* UFunKEngineSubsystem::GetTestRunner() const
{
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

	IFunKModuleInterface::Get().EngineSubsystemIsReady();
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

void UFunKEngineSubsystem::CallbackTestWorldBeganPlay(UWorld* world) const
{
	if(ActiveTestRun)
		ActiveTestRun->SetWorld(world);
}

bool UFunKEngineSubsystem::IsSeparateTestingProcess()
{
	return FString(FCommandLine::Get()).Contains(FFunKModule::FunkTestStartParameter);
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

void UFunKEngineSubsystem::FunKDebug(FString msg)
{
	GEngine->AddOnScreenDebugMessage(-1, 15000, FColor::Red, (FString("SUBSYSTEM SAYS: ") + msg));
	UE_LOG(FunKLog, Warning, TEXT("SUBSYSTEM SAYS: %s"), *msg);
}

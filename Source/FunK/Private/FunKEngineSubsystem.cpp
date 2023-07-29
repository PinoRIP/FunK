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

void UFunKEngineSubsystem::CheckForWorldController(UWorld* world)
{
	if(IsSeparateTestingProcess())
	{
		if(!IsRunning())
		{
			ConnectTestRunner(false);// TODO: is IsRemote event important? (The idea was that we could use the session frontend to start a process on a remote computer for even more realistic tests... But do we need to know this is the case?)
		}
	}
	
	ActiveTestRun->SetWorld(world);
}

void UFunKEngineSubsystem::RegisterController(AFunKWorldTestController* controller) const
{
	ActiveTestRun->RegisterWorldController(controller);
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

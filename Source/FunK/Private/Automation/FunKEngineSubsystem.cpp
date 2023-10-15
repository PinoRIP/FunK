// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKEngineSubsystem.h"
#include "FunKSettingsObject.h"
#include "FunKTestRunner.h"
#include "FunK.h"
#include "FunKLogging.h"

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

bool UFunKEngineSubsystem::HasTestRunner() const
{
	return !!ActiveTestRun;
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

bool UFunKEngineSubsystem::IsSeparateTestingProcess()
{
	return FString(FCommandLine::Get()).Contains(FFunKModule::FunkTestStartParameter);
}

UFunKTestRunner* UFunKEngineSubsystem::SetupTestRun(EFunKTestRunnerType RunType)
{
	EndTestRun();

	const UClass* TestRunnerClass = GetDefault<UFunKSettingsObject>()->Settings.TestRunnerClassOverride.Get();
	ActiveTestRun = TestRunnerClass
		? NewObject<UFunKTestRunner>(this, TestRunnerClass)
		: NewObject<UFunKTestRunner>(this);

	ActiveTestRun->Init(this, RunType);

	return ActiveTestRun;
}

void UFunKEngineSubsystem::FunKDebug(FString msg)
{
	GEngine->AddOnScreenDebugMessage(-1, 15000, FColor::Red, (FString("SUBSYSTEM SAYS: ") + msg));
	UE_LOG(FunKLog, Warning, TEXT("SUBSYSTEM SAYS: %s"), *msg);
}

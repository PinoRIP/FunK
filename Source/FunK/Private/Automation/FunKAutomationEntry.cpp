﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKAutomationEntry.h"
#include "FunKEngineSubsystem.h"
#include "FunKAutomationLatentTestRunCommand.h"
#include "FunKAutomationLatentWaitForTestRunCommand.h"
#include "CoreMinimal.h"
#include "FunK.h"

#define ENSURE(Ptr) if (!Ptr) { AddError(TEXT(#Ptr " not found!")); return false; }


void FFunKAutomationEntry::ParseTestMapInfo(const FString& Parameters, FString& MapObjectPath, FString& MapPackageName, FString& MapTestName, FString& Params)
{
	TArray<FString> ParamArray;
	Parameters.ParseIntoArray(ParamArray, TEXT(";"), true);

	MapObjectPath = ParamArray[0];
	MapPackageName = ParamArray[1];
	MapTestName = (ParamArray.Num() > 2) ? ParamArray[2] : TEXT("");
	Params = (ParamArray.Num() > 3) ? ParamArray[3] : TEXT("");
}

void FFunKAutomationEntry::StartTest(UFunKEngineSubsystem* EngineSubsystem, const FFunKTestInstructions& TestInstructions)
{
	UFunKTestRunner* TestRunner = EngineSubsystem->HasTestRunner()
		? EngineSubsystem->GetTestRunner()
		: EngineSubsystem->StartTestRunner();
	
	TestRunner->Start(TestInstructions);
	ADD_LATENT_AUTOMATION_COMMAND(FFunKAutomationLatentTestRunCommand(TestRunner))
}

IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FFunKAutomationEntryRuntime, FFunKAutomationEntry, "FunK", (EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter))

void FFunKAutomationEntryRuntime::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	TArray<FString> MapAssets;
	IFunKModuleInterface::Get().GetTests(false, OutBeautifiedNames, OutTestCommands, MapAssets);
}

bool FFunKAutomationEntryRuntime::RunTest(const FString& Parameters)
{
	ENSURE(GEngine);

	UFunKEngineSubsystem* EngineSubsystem = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>();
	ENSURE(EngineSubsystem);
	
	FString MapObjectPath, MapPackageName, MapTestName, Params;
	ParseTestMapInfo(Parameters, MapObjectPath, MapPackageName, MapTestName, Params);
	const FFunKTestInstructions Instructions(MapObjectPath, MapPackageName, MapTestName, Params);
		
	if (!EngineSubsystem->IsRunning())
	{
		StartTest(EngineSubsystem, Instructions);
		return true;
	}
	else
	{
		ADD_LATENT_AUTOMATION_COMMAND(FFunKAutomationLatentWaitForTestRunCommand(EngineSubsystem, Instructions))
		return true;
	}
}

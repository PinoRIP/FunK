// Fill out your copyright notice in the Description page of Project Settings.

#include "Automation/FunKAutomationEntry.h"
#include "CoreMinimal.h"
#include "Editor.h"
#include "FunK.h"
#include "FunKEngineSubsystem.h"
#include "Automation/FunKAutomationLatentTestRunCommand.h"

void FFunKAutomationEntry::ParseTestMapInfo(const FString& Parameters, FString& MapObjectPath, FString& MapPackageName, FString& MapTestName, FString& Params)
{
	TArray<FString> ParamArray;
	Parameters.ParseIntoArray(ParamArray, TEXT(";"), true);

	MapObjectPath = ParamArray[0];
	MapPackageName = ParamArray[1];
	MapTestName = (ParamArray.Num() > 2) ? ParamArray[2] : TEXT("");
	Params = (ParamArray.Num() > 3) ? ParamArray[3] : TEXT("");
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FFunKAutomationEntryRuntime, FFunKAutomationEntry, "FunK", (EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter))

void FFunKAutomationEntryRuntime::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	TArray<FString> MapAssets;
	IFunKModuleInterface::Get().GetTests(false, OutBeautifiedNames, OutTestCommands, MapAssets);
}

bool FFunKAutomationEntryRuntime::RunTest(const FString& Parameters)
{
	if(GEditor)
	{
		UFunKEngineSubsystem* EngineSubsystem = GEditor->GetEngineSubsystem<UFunKEngineSubsystem>();
		
		FString MapObjectPath, MapPackageName, MapTestName, Params;
		ParseTestMapInfo(Parameters, MapObjectPath, MapPackageName, MapTestName, Params);

		UFunKTestRunner* testRunner = EngineSubsystem->IsRunning()
			? EngineSubsystem->GetTestRunner()
			: EngineSubsystem->StartTestRunner();
		
		if(testRunner)
		{
			ADD_LATENT_AUTOMATION_COMMAND(FFunKAutomationLatentTestRunCommand(testRunner, FFunKTestInstructions(MapObjectPath, MapPackageName, MapTestName, Params)))
			return true;
		}
		else
		{
			AddError("No test runner found...");
		}
	}

	return false;
}

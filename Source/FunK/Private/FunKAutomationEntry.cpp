// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKAutomationEntry.h"
#include "CoreMinimal.h"
#include "Editor.h"
#include "FunK.h"
#include "FunKEngineSubsystem.h"
#include "Commands/FunKAutomationLatentTestRunCommand.h"

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

	IFunKModuleInterface::GetPtr()->SetTemp();
}

bool FFunKAutomationEntryRuntime::RunTest(const FString& Parameters)
{
	if(GEditor)
	{
		FString MapObjectPath, MapPackageName, MapTestName, Params;
		ParseTestMapInfo(Parameters, MapObjectPath, MapPackageName, MapTestName, Params);

		UFunKEngineSubsystem* EngineSubsystem = GEditor->GetEngineSubsystem<UFunKEngineSubsystem>();
		UFunKTestRunner* testRunner = EngineSubsystem->GetTestRunner();
		if(testRunner)
		{
			bool bCanProceed = testRunner->Prepare(FFunKTestInstructions(MapObjectPath, MapPackageName, MapTestName, Params));
			if(bCanProceed)
			{
				ADD_LATENT_AUTOMATION_COMMAND(FFunKAutomationLatentTestRunCommand(testRunner))
				return true;
			}
		}
		else
		{
			AddError("No test runner found...");
		}
	}
	
	//FString MapObjectPath, MapPackageName, MapTestName;
	//ParseTestMapInfo(Parameters, MapObjectPath, MapPackageName, MapTestName);
	return false;
	/*bool bCanProceed = false;

	IFunctionalTestingModule::Get().MarkPendingActivation();

	// Always reset these, even though tests should do the same
	SetLogErrorAndWarningHandlingToDefault();

	UWorld* TestWorld = GetAnyGameWorld();
	if (TestWorld && TestWorld->GetMapName() == MapPackageName)
	{
		// Map is already loaded.
		bCanProceed = true;
	}
	else
	{
		bCanProceed = AutomationOpenMap(MapPackageName);
	}

	if (bCanProceed)
	{
		if (MapTestName.IsEmpty())
		{
			ADD_LATENT_AUTOMATION_COMMAND(FStartFTestsOnMap());
		}
		else
		{
			ADD_LATENT_AUTOMATION_COMMAND(FStartFTestOnMap(MapTestName));
		}

		return true;
	}

	/// FAutomationTestFramework::GetInstance().UnregisterAutomationTest

	//	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
	//  ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);

	UE_LOG(LogFunctionalTest, Error, TEXT("Failed to start the %s map (possibly due to BP compilation issues)"), *MapPackageName);
	return false;
	*/
}

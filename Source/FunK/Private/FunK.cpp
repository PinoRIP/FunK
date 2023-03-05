// Copyright Epic Games, Inc. All Rights Reserved.

#include "FunK.h"
#include "EngineUtils.h"
#include "FunKEngineSubsystem.h"
#include "FunKFunctionalTest.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/AutomationTest.h"

#define LOCTEXT_NAMESPACE "FFunKModule"

class FAssetRegistryModule;
class IAssetRegistry;
FName FFunKModule::FunkTestWorldTag = FName("FunKTests");
FName FFunKModule::FunkEditorOnlyTestWorldTag = FName("FunKEditorOnlyTests");

FString FFunKModule::FunkStandaloneParameter = FString("-standalone");
FString FFunKModule::FunkListenParameter = FString("-listen");
FString FFunKModule::FunkDedicatedParameter = FString("-dedicated");

FString FFunKModule::FunkTestStartParameter = FString("-funk-test");

void FFunKModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	FWorldDelegates::GetAssetTags.AddRaw(this, &FFunKModule::OnWorldGetAssetTags);
#endif
}

void FFunKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_EDITOR
	FWorldDelegates::GetAssetTags.RemoveAll(this);
	FAutomationTestFramework::Get().PreTestingEvent.RemoveAll(this);
	FAutomationTestFramework::Get().PostTestingEvent.RemoveAll(this);
#endif
}

void FFunKModule::GetTests(bool bEditorOnlyTests, TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands, TArray<FString>& OutTestMapAssets) const
{
	IAssetRegistry& AssetRegistry = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	if (!AssetRegistry.IsLoadingAssets())
	{
#if WITH_EDITOR
		static bool bDidScan = false;

		if (!GIsEditor && !bDidScan)
		{
			// For editor build -game, we need to do a full scan
			AssetRegistry.SearchAllAssets(true);
			bDidScan = true;
		}
#endif

		TArray<FAssetData> MapList;
		FARFilter Filter;
		Filter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());
		Filter.bRecursiveClasses = true;
		Filter.bIncludeOnlyOnDiskAssets = true;
		if (AssetRegistry.GetAssets(Filter, /*out*/ MapList))
		{
			for (const FAssetData& MapAsset : MapList)
			{
				FString MapAssetPath = MapAsset.GetObjectPathString();
				FString MapPackageName = MapAsset.PackageName.ToString();
				FString PartialSuiteName = MapPackageName.RightChop(1).Replace(TEXT("/"), TEXT(".")); // use dot syntax
				if (MapPackageName.StartsWith(TEXT("/Game/")))
				{
					PartialSuiteName = PartialSuiteName.RightChop(5); // Remove "/Game/" from the name
				}

				FString AllTestNames;
				FAssetDataTagMapSharedView::FFindTagResult MapTestNames = MapAsset.TagsAndValues.FindTag(bEditorOnlyTests ? FunkEditorOnlyTestWorldTag : FunkTestWorldTag);

				if (MapTestNames.IsSet())
				{
					AllTestNames = MapTestNames.GetValue();
				}

#if WITH_EDITOR
				// Also append external functional test actors
				if (ULevel::GetIsLevelUsingExternalActorsFromAsset(MapAsset))
				{
					const FString LevelExternalActorsPath = ULevel::GetExternalActorsPath(MapPackageName);

					// Do a synchronous scan of the level external actors path.			
					AssetRegistry.ScanPathsSynchronous({ LevelExternalActorsPath }, /*bForceRescan*/false, /*bIgnoreDenyListScanFilters*/false);

					FARFilter ActorsFilter;
					ActorsFilter.bRecursivePaths = true;
					ActorsFilter.bIncludeOnlyOnDiskAssets = true;
					ActorsFilter.PackagePaths.Add(*LevelExternalActorsPath);

					TArray<FAssetData> ActorList;
					AssetRegistry.GetAssets(ActorsFilter, ActorList);

					for (const FAssetData& ActorAsset : ActorList)
					{
						FAssetDataTagMapSharedView::FFindTagResult ActorTestName = ActorAsset.TagsAndValues.FindTag(bEditorOnlyTests ? FunkEditorOnlyTestWorldTag : FunkTestWorldTag);

						if (ActorTestName.IsSet())
						{
							if (!AllTestNames.IsEmpty())
							{
								AllTestNames += TEXT(";");
							}

							AllTestNames += ActorTestName.GetValue();
						}
					}
				}
#endif
				if (!AllTestNames.IsEmpty())
				{
					TArray<FString> MapTests;
					AllTestNames.ParseIntoArray(MapTests, TEXT(";"), true);

					for (const FString& MapTest : MapTests)
					{
						FString BeautifulTestName;
						FString TestNameAndParams;

						if (MapTest.Split(TEXT("|"), &BeautifulTestName, &TestNameAndParams))
						{
							FString RealTestName;
							FString Params;
							
							if (TestNameAndParams.Split(TEXT(":"), &Params, &RealTestName))
							{
								TArray<FString> RunOns;
								Params.ParseIntoArray(RunOns, TEXT("-"), true);

								for (const FString& RunOn : RunOns)
								{
									OutBeautifiedNames.Add(RunOn + TEXT(".") + PartialSuiteName + TEXT(".") + *BeautifulTestName);
									OutTestCommands.Add(MapAssetPath + TEXT(";") + MapPackageName + TEXT(";") + *RealTestName + TEXT(";-") + *RunOn);
									OutTestMapAssets.AddUnique(MapAssetPath);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool FFunKModule::IsActive() const
{
	return true;
}

void FFunKModule::EngineSubsystemIsReady()
{
#if WITH_EDITOR
	FAutomationTestFramework::Get().PreTestingEvent.AddRaw(this, &FFunKModule::Pre);
	FAutomationTestFramework::Get().PostTestingEvent.AddRaw(this, &FFunKModule::Post);
#endif
}

void FFunKModule::OnWorldGetAssetTags(const UWorld* World, TArray<UObject::FAssetRegistryTag>& OutTags)
{
	if(IsActive())
	{
		FString TestNames, TestNamesEditor;
		for (TActorIterator<AFunKFunctionalTest> ActorItr(World, AFunKFunctionalTest::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
		{
			AFunKFunctionalTest* FunctionalTest = *ActorItr;

			if (!FunctionalTest->IsPackageExternal())
			{
				bool bIsEditorOnly = IsEditorOnlyObject(FunctionalTest);

				// Check if this class is editor only
				FString& NamesAppend = bIsEditorOnly ? TestNamesEditor : TestNames;
				FunctionalTest->BuildTestRegistry(NamesAppend);
			}
		}
		
		if (!TestNames.IsEmpty())
		{
			OutTags.Add(UObject::FAssetRegistryTag(FunkTestWorldTag, TestNames, UObject::FAssetRegistryTag::TT_Hidden));
		}

		if (!TestNamesEditor.IsEmpty())
		{
			OutTags.Add(UObject::FAssetRegistryTag(FunkEditorOnlyTestWorldTag, TestNamesEditor, UObject::FAssetRegistryTag::TT_Hidden));
		}
	}
}

void FFunKModule::Pre()
{
	if(UFunKEngineSubsystem* engineSubsystem = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>())
	{
		engineSubsystem->StartTestRunner();
	}
}

void FFunKModule::Post()
{
	if(UFunKEngineSubsystem* engineSubsystem = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>())
	{
		engineSubsystem->EndTestRun();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFunKModule, FunK)
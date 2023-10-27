// Copyright Epic Games, Inc. All Rights Reserved.

#include "FunK.h"
#include "EngineUtils.h"
#include "FunKFunctionalTest.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "FunKSettingsObject.h"

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
#if WITH_EDITOR
	// register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "FunK",
			LOCTEXT("FunKSettingsName", "FunK Settings"),
			LOCTEXT("FunKSettingsNameDescription", "Configure the FunK automated tests"),
			GetMutableDefault<UFunKSettingsObject>()
		);
	}
	
	FWorldDelegates::GetAssetTags.AddRaw(this, &FFunKModule::OnWorldGetAssetTags);
#endif
}

void FFunKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "FunK");
	}
	
	FWorldDelegates::GetAssetTags.RemoveAll(this);
#endif
}

void FFunKModule::GetTests(bool bEditorOnlyTests, TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands, TArray<FString>& OutTestMapAssets) const
{
	IAssetRegistry& AssetRegistry = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	if (!AssetRegistry.IsLoadingAssets())
	{
#if WITH_EDITOR
		static bool IsScanned = false;

		if (!GIsEditor && !IsScanned)
		{
			// For editor build -game, we need to do a full scan
			AssetRegistry.SearchAllAssets(true);
			IsScanned = true;
		}
#endif

		TArray<FAssetData> MapList;
		GetTestMapAssets(AssetRegistry, MapList);
		for (const FAssetData& MapAsset : MapList)
		{
			FString MapAssetPath = MapAsset.GetObjectPathString();
			FString MapPackageName = MapAsset.PackageName.ToString();
			FString PartialSuiteName = MapPackageName.RightChop(1).Replace(TEXT("/"), TEXT("."));
			if (MapPackageName.StartsWith(TEXT("/Game/")))
			{
				// Remove "/Game/" path section from the name
				PartialSuiteName = PartialSuiteName.RightChop(5);
			}

			FString AllTestNames;
			FAssetDataTagMapSharedView::FFindTagResult MapTestNames = MapAsset.TagsAndValues.FindTag(bEditorOnlyTests ? FunkEditorOnlyTestWorldTag : FunkTestWorldTag);

			if (MapTestNames.IsSet())
			{
				AllTestNames = MapTestNames.GetValue();
			}

#if WITH_EDITOR
			if (ULevel::GetIsLevelUsingExternalActorsFromAsset(MapAsset))
			{
				const FString LevelExternalActorsPath = ULevel::GetExternalActorsPath(MapPackageName);
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
								OutBeautifiedNames.Add(RunOn + TEXT(".") + PartialSuiteName + TEXT(".") + *BeautifulTestName + TEXT(" -") + *RunOn);
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

bool FFunKModule::IsActive() const
{
	return true;
}

void FFunKModule::OnWorldGetAssetTags(const UWorld* World, TArray<UObject::FAssetRegistryTag>& OutTags) const
{
	if (IsActive())
	{
		FString TestNames, TestNamesEditor;
		for (TActorIterator<AFunKTestBase> ActorItr(World, AFunKTestBase::StaticClass(), EActorIteratorFlags::AllActors); ActorItr; ++ActorItr)
		{
			const AFunKTestBase* FunctionalTest = *ActorItr;

			if (!FunctionalTest->IsPackageExternal())
			{
				const bool IsEditorOnly = IsEditorOnlyObject(FunctionalTest);

				// Check if this class is editor only
				FString& NamesAppend = IsEditorOnly ? TestNamesEditor : TestNames;
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

void FFunKModule::GetTestMapAssets(const IAssetRegistry& AssetRegistry, TArray<FAssetData>& MapList) const
{
	const FFunKSettings& Settings = GetDefault<UFunKSettingsObject>()->Settings;

	FARFilter Filter;
	Filter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;
	Filter.bIncludeOnlyOnDiskAssets = true;
	
	TArray<FAssetData> AllMaps;
	if (!AssetRegistry.GetAssets(Filter, /*out*/ AllMaps))
		return;

	if (Settings.DiscoveryMethod == EFunKTestDiscoveryMethod::Search)
	{
		MapList = AllMaps;
	}
	else if (Settings.DiscoveryMethod == EFunKTestDiscoveryMethod::Prefix)
	{
		for (FAssetData& Map : AllMaps)
		{
			if (Map.AssetName.ToString().StartsWith(Settings.Prefix))
				MapList.Add(Map);
		}
	}
	else if (Settings.DiscoveryMethod == EFunKTestDiscoveryMethod::Worlds)
	{
		for (const TSoftObjectPtr<UWorld>& World : Settings.Worlds)
		{
			for (int i = 0; i < AllMaps.Num(); ++i)
			{
				if (World.ToSoftObjectPath() == AllMaps[i].ToSoftObjectPath())
				{
					MapList.Add(AllMaps[i]);
					AllMaps.RemoveAt(i);
					break;
				}
			}
		}
	}
	else if (Settings.DiscoveryMethod == EFunKTestDiscoveryMethod::Paths)
	{
		for (const FString& Path : Settings.Paths)
		{
			for (FAssetData& Map : AllMaps)
			{
				if (Map.PackageName.ToString().StartsWith(Path))
					MapList.Add(Map);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFunKModule, FunK)
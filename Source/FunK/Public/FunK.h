// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetRegistry;

class IFunKModuleInterface : public IModuleInterface
{
public:
	/** Gets a list of maps/tests in the current project */
	virtual void GetTests(bool bEditorOnlyTests, TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands, TArray<FString>& OutTestMapAssets) const = 0;

	virtual bool IsActive() const = 0;

	static IFunKModuleInterface& Get()
	{
		static const FName ModuleName(TEXT("FunK"));
		return FModuleManager::Get().LoadModuleChecked<IFunKModuleInterface>(ModuleName);
	}
	
	static IFunKModuleInterface* GetPtr()
	{
		return &Get();
	}
};


class FFunKModule : public IFunKModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Gets a list of maps/tests in the current project */
	virtual void GetTests(bool bEditorOnlyTests, TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands, TArray<FString>& OutTestMapAssets) const override;

	virtual bool IsActive() const override;

	static FName FunkTestWorldTag;
	static FName FunkEditorOnlyTestWorldTag;
	
	static FString FunkStandaloneParameter;
	static FString FunkListenParameter;
	static FString FunkDedicatedParameter;
	
	static FString FunkTestStartParameter;

private:
	void OnWorldGetAssetTags(const UWorld* World, TArray<UObject::FAssetRegistryTag>& OutTags) const;

	void GetTestMapAssets(const IAssetRegistry& AssetRegistry, TArray<FAssetData>& MapList) const;
};

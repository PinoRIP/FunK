// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestRunner.h"
#include "Modules/ModuleManager.h"

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

	virtual void SetTemp() = 0;
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

	//TODO: remove this -> The Pre and Post Events get called at module startup which is too early for the subsystem to be created... Just a dirty fix here to test some stuff
	bool Temp = false;
	virtual void SetTemp() override { Temp = true; }

private:
	void OnWorldGetAssetTags(const UWorld* World, TArray<UObject::FAssetRegistryTag>& OutTags);
	void Pre();
	void Post();
};

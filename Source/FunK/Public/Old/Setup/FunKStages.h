// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Setup/FunKStage.h"
#include "FunKStages.generated.h"

// Structure that holds the references & configuration to all stages
USTRUCT(BlueprintType)
struct FUNK_API FFunKStages
{
	GENERATED_BODY()

public:
	TArray<FFunKStage> Stages;

	int32 OnStandaloneCount = 0;
	int32 OnDedicatedServerCount = 0;
	int32 OnDedicatedServerClientCount = 0;
	int32 OnListenServerCount = 0;
	int32 OnListenServerClientCount = 0;

	template <typename UserClass, typename... VarTypes>
	inline FFunKStage& EmplaceStage(const FName& StageName, const int32 Index, UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		FFunKStage& Stage = EmplaceStage(StageName, Index);
		Stage.StartDelegate.BindUObject(InUserObject, InFunc, Vars...);
		return Stage;
	}
	
	template <typename UserClass, typename... VarTypes>
	inline FFunKStage& EmplaceLatentStage(const FName& StageName, int32 Index, UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		FFunKStage& Stage = EmplaceStage(StageName, Index, InUserObject, InFunc, Vars...);
		Stage.IsLatent = true;
		return Stage;
	}

private:
	FFunKStage& EmplaceStage(const FName& StageName, const int32 Index)
	{
		FFunKStage& Stage = Stages.EmplaceAt_GetRef(Index, FFunKStage());
		Stage.Name = StageName;

		OnStandaloneCount = OnStandaloneCount + (Stage.IsOnStandalone ? 1 : 0);
		OnDedicatedServerCount = OnDedicatedServerCount + (Stage.IsOnDedicatedServer ? 1 : 0);
		OnDedicatedServerClientCount = OnDedicatedServerClientCount + (Stage.IsOnDedicatedServerClient ? 1 : 0);
		OnListenServerCount = OnListenServerCount + (Stage.IsOnListenServer ? 1 : 0);
		OnListenServerClientCount = OnListenServerClientCount + (Stage.IsOnListenServerClient ? 1 : 0);
		
		return Stage;
	}
};
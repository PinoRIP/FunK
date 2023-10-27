// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKStage.h"
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
		FFunKStage& stage = EmplaceStage(StageName, Index);
		stage.StartDelegate.BindUObject(InUserObject, InFunc, Vars...);
		return stage;
	}
	
	template <typename UserClass, typename... VarTypes>
	inline FFunKStage& EmplaceLatentStage(const FName& StageName, int32 Index, UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		FFunKStage& stage = EmplaceStage(StageName, Index, InUserObject, InFunc, Vars...);
		stage.IsLatent = true;
		return stage;
	}

private:
	FFunKStage& EmplaceStage(const FName& StageName, const int32 Index)
	{
		FFunKStage& stage = Stages.EmplaceAt_GetRef(Index, FFunKStage());
		stage.Name = StageName;

		OnStandaloneCount = OnStandaloneCount + (stage.IsOnStandalone ? 1 : 0);
		OnDedicatedServerCount = OnDedicatedServerCount + (stage.IsOnDedicatedServer ? 1 : 0);
		OnDedicatedServerClientCount = OnDedicatedServerClientCount + (stage.IsOnDedicatedServerClient ? 1 : 0);
		OnListenServerCount = OnListenServerCount + (stage.IsOnListenServer ? 1 : 0);
		OnListenServerClientCount = OnListenServerClientCount + (stage.IsOnListenServerClient ? 1 : 0);
		
		return stage;
	}
};
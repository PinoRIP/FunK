// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKStage.h"
#include "FunKStages.generated.h"

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
	inline FFunKStage& EmplaceStage(const FName& stageName, int32 index, UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		FFunKStage& stage = EmplaceStage(stageName, index);
		stage.Delegate.BindUObject(InUserObject, InFunc, Vars...);
		return stage;
	}
	
	template <typename UserClass, typename... VarTypes>
	inline FFunKStage& EmplaceLatentStage(const FName& stageName, int32 index, UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void ( VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		FFunKStage& stage = EmplaceStage(stageName, index, InUserObject, InFunc, Vars...);
		stage.IsLatent = true;
		return stage;
	}

private:
	FFunKStage& EmplaceStage(const FName& stageName, int32 index)
	{
		FFunKStage& stage = Stages.EmplaceAt_GetRef(index, FFunKStage());
		stage.Name = stageName;

		OnStandaloneCount = OnStandaloneCount + stage.IsOnStandalone ? 1 : 0;
		OnDedicatedServerCount = OnDedicatedServerCount + stage.IsOnDedicatedServer ? 1 : 0;
		OnDedicatedServerClientCount = OnDedicatedServerClientCount + stage.IsOnDedicatedServerClient ? 1 : 0;
		OnListenServerCount = OnListenServerCount + stage.IsOnListenServer ? 1 : 0;
		OnListenServerClientCount = OnListenServerClientCount + stage.IsOnListenServerClient ? 1 : 0;
		
		return stage;
	}
};
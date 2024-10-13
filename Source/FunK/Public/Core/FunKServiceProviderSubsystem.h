// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "FunKServiceProviderSubsystem.generated.h"

/**
 * Very primitive service provider to manage service instancing & lifetimes
 */
UCLASS()
class FUNK_API UFunKServiceProviderSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	template <typename TService>
	TService* Service()
	{
		return Service(TService::StaticClass());
	}

	UObject* Service(const UClass* ServiceClass);

private:
	UPROPERTY()
	TArray<UObject*> ServiceInstances;

	UObject* FindService(const UClass* ServiceClass);
	UObject* CreateService(const UClass* ServiceClass);
};

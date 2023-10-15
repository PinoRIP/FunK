// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKNetworkEmulationTypes.h"

#include "Engine/NetworkSettings.h"

TArray<FString> UFunKNetworkEmulationTypesHelper::GetProfileOptions()
{
	TArray<FString> Result;
	if (const UNetworkSettings* NetworkSettings = GetDefault<UNetworkSettings>())
	{
		for (int i = 0; i < NetworkSettings->NetworkEmulationProfiles.Num(); ++i)
		{
			Result.Add(NetworkSettings->NetworkEmulationProfiles[i].ProfileName);
		}
	}

	return Result;
}

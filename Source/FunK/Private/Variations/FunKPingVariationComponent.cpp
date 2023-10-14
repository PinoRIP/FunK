// Fill out your copyright notice in the Description page of Project Settings.


#include "Variations\FunKNetworkVariationComponent.h"
#include "FunKLogging.h"
#include "Engine/NetDriver.h"
#include "Engine/NetworkSettings.h"
#include "Util/FunKUtilBlueprintFunctionLibrary.h"


void UFunKNetworkVariationFunctionality::OnAdded()
{
	const EFunKNetLocation NetLocation = UFunKBlueprintFunctionLibrary::GetNetLocation(this);
	if(NetLocation == EFunKNetLocation::Standalone) return;

	const FFunKNetworkEmulation& Emulation = Spawner->Emulations[Index];
	if(Emulation.EmulationTarget == EFunKNetworkEmulationTarget::Client && NetLocation == EFunKNetLocation::Server) return;
	if(Emulation.EmulationTarget == EFunKNetworkEmulationTarget::Server && NetLocation == EFunKNetLocation::Client) return;
	
#if DO_ENABLE_NET_TEST
	
	FPacketSimulationSettings Settings = FPacketSimulationSettings();

	if (Emulation.UseCustomSettings)
	{
		Settings.PktLagMin = Emulation.Custom.OutPackets.MinLatency;
		Settings.PktLagMax = Emulation.Custom.OutPackets.MaxLatency;
		Settings.PktLoss = Emulation.Custom.OutPackets.PacketLossPercentage;
		Settings.PktIncomingLagMin = Emulation.Custom.InPackets.MinLatency;
		Settings.PktIncomingLagMax = Emulation.Custom.InPackets.MaxLatency;
		Settings.PktIncomingLoss = Emulation.Custom.InPackets.PacketLossPercentage;
	}
	else
	{
		if (!Settings.LoadEmulationProfile(*Emulation.Profile))
		{
			Error("Missing network emulation profile: " + Emulation.Profile);
			return;
		}
	}

	FWorldContext& Context = GEngine->GetWorldContextFromWorldChecked(GetWorld());
	for (const FNamedNetDriver& ActiveNetDriver : Context.ActiveNetDrivers)
	{
		if (ActiveNetDriver.NetDriver)
		{
			InitialDriverSettings.Add(TKeyValuePair(TWeakObjectPtr<UNetDriver>(ActiveNetDriver.NetDriver), ActiveNetDriver.NetDriver->PacketSimulationSettings));
			ActiveNetDriver.NetDriver->SetPacketSimulationSettings(Settings);
		}
	}
#else

	Error("Network emulation is not enabled!");

#endif
}

void UFunKNetworkVariationFunctionality::OnRemoved()
{
#if DO_ENABLE_NET_TEST
	for (TKeyValuePair<TWeakObjectPtr<UNetDriver>, FPacketSimulationSettings>& InitialDriverSetting : InitialDriverSettings)
	{
		if (!InitialDriverSetting.Key.IsValid())
			continue;

		InitialDriverSetting.Key->SetPacketSimulationSettings(InitialDriverSetting.Value);
	}
#endif

	InitialDriverSettings.Empty();
}

FString UFunKNetworkVariationFunctionality::GetReadableIdent() const
{
	if (Index == INDEX_NONE || Spawner.IsStale()) return Super::GetName();
	
	const FFunKNetworkEmulation& Emulation = Spawner->Emulations[Index];
	return Emulation.UseCustomSettings
		? ("Custom " + Emulation.Custom.Name)
		: ("Profile " + Emulation.Profile);
}

// Sets default values for this component's properties
UFunKNetworkVariationComponent::UFunKNetworkVariationComponent()
{
}

int32 UFunKNetworkVariationComponent::GetCount()
{
#if DO_ENABLE_NET_TEST
	return Emulations.Num();
#else
	return 0;
#endif
}

UFunKTestFunctionality* UFunKNetworkVariationComponent::GetFunctionality(int32 Index)
{
	UFunKNetworkVariationFunctionality* Functionality = NewObject<UFunKNetworkVariationFunctionality>(this);
	Functionality->Index = Index;
	Functionality->Spawner = this;
	return Functionality;
}

TArray<FString> UFunKNetworkVariationComponent::GetProfileOptions()
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



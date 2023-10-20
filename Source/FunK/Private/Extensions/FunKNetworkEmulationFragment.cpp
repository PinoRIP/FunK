// Fill out your copyright notice in the Description page of Project Settings.


#include "Extensions/FunKNetworkEmulationFragment.h"
#include "Util/FunKNetworkEmulationTypes.h"
#include "Util/FunKUtilBlueprintFunctionLibrary.h"
#include "Util/FunKUtilTypes.h"
#include "Engine/NetDriver.h"


void UFunKNetworkEmulationFragment::OnAdded()
{
	const EFunKNetLocation NetLocation = UFunKBlueprintFunctionLibrary::GetNetLocation(this);
	if(NetLocation == EFunKNetLocation::Standalone) return;

	const FFunKNetworkEmulation& Emulation = NetworkEmulation;
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

void UFunKNetworkEmulationFragment::OnRemoved()
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

FString UFunKNetworkEmulationFragment::GetReadableIdent() const
{
	const FFunKNetworkEmulation& Emulation = NetworkEmulation;
	return Emulation.UseCustomSettings
		? ("Custom " + Emulation.Custom.Name)
		: ("Profile " + Emulation.Profile);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKUtilBlueprintFunctionLibrary.h"
#include "FunKWorldSubsystem.h"

UFunKWorldSubsystem* UFunKBlueprintFunctionLibrary::GetFunKWorldSubsystem(const UObject* WorldContext)
{
	if (!WorldContext)
		return nullptr;

	return WorldContext->GetWorld()->GetSubsystem<UFunKWorldSubsystem>();
}

int32 UFunKBlueprintFunctionLibrary::GetPeerIndex(const UObject* WorldContext)
{
	if (const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->GetPeerIndex();
	}

	return INDEX_NONE;
}

EFunKNetMode UFunKBlueprintFunctionLibrary::GetNetMode(const UObject* WorldContext)
{
	if (!WorldContext)
		return EFunKNetMode::MAX;
	
	switch (WorldContext->GetWorld()->GetNetMode()) {
	case NM_DedicatedServer: return EFunKNetMode::DedicatedServer;
	case NM_ListenServer: return EFunKNetMode::ListenServer;
	case NM_Standalone: return EFunKNetMode::Standalone;
	case NM_Client: return EFunKNetMode::Client;
	case NM_MAX: return EFunKNetMode::MAX;
	default: return EFunKNetMode::MAX;
	}
}

void UFunKBlueprintFunctionLibrary::SwitchNetMode(const UObject* WorldContext, EFunKNetMode& Branches)
{
	Branches = GetNetMode(WorldContext);
}

EFunKClient UFunKBlueprintFunctionLibrary::GetClients(const UObject* WorldContext)
{
	const EFunKNetMode NetMode = GetNetMode(WorldContext);
	if (NetMode == EFunKNetMode::Standalone)
	{
		return EFunKClient::Host;
	}
	
	if (NetMode == EFunKNetMode::ListenServer)
	{
		return EFunKClient::Host;
	}

	if (NetMode == EFunKNetMode::Client)
	{
		const int32 PeerIndex = GetPeerIndex(WorldContext);
		if (PeerIndex == 1)
			return EFunKClient::First;

		if (PeerIndex == 2)
			return EFunKClient::Second;
		
		return EFunKClient::Any;
	}

	return EFunKClient::None;
}

void UFunKBlueprintFunctionLibrary::SwitchClients(const UObject* WorldContext, EFunKClient& Branches)
{
	Branches = GetClients(WorldContext);
}

EFunKTestEnvironmentType UFunKBlueprintFunctionLibrary::GetTestEnvironmentType(const UObject* WorldContext)
{
	const EFunKNetMode NetMode = GetNetMode(WorldContext);
	if (NetMode == EFunKNetMode::Standalone)
	{
		return EFunKTestEnvironmentType::Standalone;
	}

	if (NetMode == EFunKNetMode::ListenServer)
	{
		return EFunKTestEnvironmentType::ListenServer;
	}

	if (NetMode == EFunKNetMode::DedicatedServer)
	{
		return EFunKTestEnvironmentType::DedicatedServer;
	}

	if (NetMode == EFunKNetMode::Client)
	{
		return IsServerDedicated(WorldContext)
			? EFunKTestEnvironmentType::DedicatedServer
			: EFunKTestEnvironmentType::ListenServer;
	}

	return EFunKTestEnvironmentType::MAX;
}

void UFunKBlueprintFunctionLibrary::SwitchTestEnvironmentType(const UObject* WorldContext, EFunKTestEnvironmentType& Branches)
{
	Branches = GetTestEnvironmentType(WorldContext);
}

bool UFunKBlueprintFunctionLibrary::IsServerDedicated(const UObject* WorldContext)
{
	if (const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->IsServerDedicated();
	}

	return false;
}

EFunKNetLocation UFunKBlueprintFunctionLibrary::GetNetLocation(const UObject* WorldContext)
{
	const EFunKNetMode NetMode = GetNetMode(WorldContext);
	if (NetMode == EFunKNetMode::Standalone)
	{
		return EFunKNetLocation::Standalone;
	}

	if (NetMode == EFunKNetMode::ListenServer || NetMode == EFunKNetMode::DedicatedServer)
	{
		return EFunKNetLocation::Server;
	}

	if (NetMode == EFunKNetMode::Client)
	{
		return EFunKNetLocation::Client;
	}

	return EFunKNetLocation::MAX;
}

void UFunKBlueprintFunctionLibrary::SwitchNetLocation(const UObject* WorldContext, EFunKNetLocation& Branches)
{
	Branches = GetNetLocation(WorldContext);
}

﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKBlueprintFunctionLibrary.h"
#include "FunKWorldSubsystem.h"

UFunKWorldSubsystem* UFunKBlueprintFunctionLibrary::GetFunKWorldSubsystem(UObject* WorldContext)
{
	if(!WorldContext)
		return nullptr;

	return WorldContext->GetWorld()->GetSubsystem<UFunKWorldSubsystem>();
}

int32 UFunKBlueprintFunctionLibrary::GetPeerIndex(UObject* WorldContext)
{
	if(const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->GetPeerIndex();
	}

	return INDEX_NONE;
}

EFunKNetMode UFunKBlueprintFunctionLibrary::GetNetMode(UObject* WorldContext)
{
	if(!WorldContext)
		return EFunKNetMode::MAX;
	
	switch (WorldContext->GetWorld()->GetNetMode()) {
	case NM_DedicatedServer: return EFunKNetMode::DedicatedServer;
	case NM_ListenServer: return EFunKNetMode::ListenServer;
	case NM_Standalone: return  EFunKNetMode::Standalone;
	case NM_Client: return EFunKNetMode::Client;
	case NM_MAX: return EFunKNetMode::MAX;
	default: return EFunKNetMode::MAX;
	}
}

void UFunKBlueprintFunctionLibrary::SwitchNetMode(UObject* WorldContext, EFunKNetMode& Branches)
{
	Branches = GetNetMode(WorldContext);
}

EFunKClient UFunKBlueprintFunctionLibrary::GetClients(UObject* WorldContext)
{
	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		return EFunKClient::First;
	}
	
	if (netMode == EFunKNetMode::ListenServer)
	{
		return EFunKClient::Host;
	}

	if (netMode == EFunKNetMode::Client)
	{
		const int32 number = GetPeerIndex(WorldContext);
		if (number == 1)
			return EFunKClient::First;

		if (number == 2)
			return EFunKClient::Second;
		
		return EFunKClient::Any;
	}

	return EFunKClient::None;
}

void UFunKBlueprintFunctionLibrary::SwitchClients(UObject* WorldContext, EFunKClient& Branches)
{
	Branches = GetClients(WorldContext);
}

EFunKTestEnvironmentType UFunKBlueprintFunctionLibrary::GetTestEnvironmentType(UObject* WorldContext)
{
	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		return EFunKTestEnvironmentType::Standalone;
	}

	if (netMode == EFunKNetMode::ListenServer)
	{
		return EFunKTestEnvironmentType::ListenServer;
	}

	if (netMode == EFunKNetMode::DedicatedServer)
	{
		return EFunKTestEnvironmentType::DedicatedServer;
	}

	if (netMode == EFunKNetMode::Client)
	{
		return IsServerDedicated(WorldContext)
			? EFunKTestEnvironmentType::DedicatedServer
			: EFunKTestEnvironmentType::ListenServer;
	}

	return EFunKTestEnvironmentType::MAX;
}

void UFunKBlueprintFunctionLibrary::SwitchTestEnvironmentType(UObject* WorldContext, EFunKTestEnvironmentType& Branches)
{
	Branches = GetTestEnvironmentType(WorldContext);
}

bool UFunKBlueprintFunctionLibrary::IsServerDedicated(UObject* WorldContext)
{
	if(const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->IsServerDedicated();
	}

	return false;
}

EFunKNetLocation UFunKBlueprintFunctionLibrary::GetNetLocation(UObject* WorldContext)
{
	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		return EFunKNetLocation::Standalone;
	}

	if (netMode == EFunKNetMode::ListenServer || netMode == EFunKNetMode::DedicatedServer)
	{
		return EFunKNetLocation::Server;
	}

	if (netMode == EFunKNetMode::Client)
	{
		return EFunKNetLocation::Client;
	}

	return EFunKNetLocation::MAX;
}

void UFunKBlueprintFunctionLibrary::SwitchNetLocation(UObject* WorldContext, EFunKNetLocation& Branches)
{
	Branches = GetNetLocation(WorldContext);
}

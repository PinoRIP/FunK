// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKBlueprintFunctionLibrary.h"
#include "FunKWorldSubsystem.h"

UFunKWorldSubsystem* UFunKBlueprintFunctionLibrary::GetFunKWorldSubsystem(UObject* WorldContext)
{
	if(!WorldContext)
		return nullptr;

	return WorldContext->GetWorld()->GetSubsystem<UFunKWorldSubsystem>();
}

int32 UFunKBlueprintFunctionLibrary::GetRoleNumber(UObject* WorldContext)
{
	if(const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->GetRoleNum();
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

void UFunKBlueprintFunctionLibrary::SwitchClients(UObject* WorldContext, EFunKClient& Branches)
{
	Branches = EFunKClient::None;

	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		Branches = EFunKClient::First;
	}
	else if (netMode == EFunKNetMode::ListenServer)
	{
		Branches = EFunKClient::Host;
	}
	else if (netMode == EFunKNetMode::Client)
	{
		const int32 number = GetRoleNumber(WorldContext);
		if (number == 1)
			Branches = EFunKClient::First;
		else if (number == 2)
			Branches = EFunKClient::Second;
		else
			Branches = EFunKClient::Any;
	}
}

void UFunKBlueprintFunctionLibrary::SwitchTestEnvironmentType(UObject* WorldContext, EFunKTestEnvironmentType& Branches)
{
	Branches = EFunKTestEnvironmentType::MAX;
	
	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		Branches = EFunKTestEnvironmentType::Standalone;
	}
	else if (netMode == EFunKNetMode::ListenServer)
	{
		Branches = EFunKTestEnvironmentType::ListenServer;
	}
	else if (netMode == EFunKNetMode::DedicatedServer)
	{
		Branches = EFunKTestEnvironmentType::DedicatedServer;
	}
	else if (netMode == EFunKNetMode::Client)
	{
		Branches = IsServerDedicated(WorldContext)
			? EFunKTestEnvironmentType::DedicatedServer
			: EFunKTestEnvironmentType::ListenServer;
	}
}

bool UFunKBlueprintFunctionLibrary::IsServerDedicated(UObject* WorldContext)
{
	if(const UFunKWorldSubsystem* Subsystem = GetFunKWorldSubsystem(WorldContext))
	{
		return Subsystem->IsServerDedicated();
	}

	return false;
}

void UFunKBlueprintFunctionLibrary::SwitchNetLocation(UObject* WorldContext, EFunKNetLocation& Branches)
{
	Branches = EFunKNetLocation::MAX;
	
	const auto netMode = GetNetMode(WorldContext);
	if (netMode == EFunKNetMode::Standalone)
	{
		Branches = EFunKNetLocation::Standalone;
	}
	else if (netMode == EFunKNetMode::ListenServer || netMode == EFunKNetMode::DedicatedServer)
	{
		Branches = EFunKNetLocation::Server;
	}
	else if (netMode == EFunKNetMode::Client)
	{
		Branches = EFunKNetLocation::Client;
	}
}

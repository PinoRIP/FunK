// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestLocationTarget.h"
#include "UObject/Object.h"
#include "FunKActorScenario.generated.h"

USTRUCT()
struct FFunKActorScenario
{
	GENERATED_BODY()

public:
	FFunKActorScenario()
		: FFunKActorScenario(EFunKTestLocationTarget::None, EFunKTestLocationTarget::None)
	{ }
	
	FFunKActorScenario(EFunKTestLocationTarget from, EFunKTestLocationTarget to)
		: From(from)
		, To(to)
	{ }

	EFunKTestLocationTarget GetFrom() const { return From; }
	EFunKTestLocationTarget GetTo() const { return To; }

	bool IsListenServer() const { return (From == EFunKTestLocationTarget::ListenServer || From == EFunKTestLocationTarget::ListenServerClient) && (To == EFunKTestLocationTarget::ListenServer || To == EFunKTestLocationTarget::ListenServerClient); }
	bool IsDedicatedServer() const { return (From == EFunKTestLocationTarget::DedicatedServer || From == EFunKTestLocationTarget::DedicatedServerClient) && (To == EFunKTestLocationTarget::DedicatedServer || To == EFunKTestLocationTarget::DedicatedServerClient); }
	bool IsServerToClient() const { return (From == EFunKTestLocationTarget::ListenServer || From == EFunKTestLocationTarget::DedicatedServer) && (To == EFunKTestLocationTarget::ListenServerClient || To == EFunKTestLocationTarget::DedicatedServerClient); }
	bool IsClientToServer() const { return (From == EFunKTestLocationTarget::ListenServerClient || From == EFunKTestLocationTarget::DedicatedServerClient) && (To == EFunKTestLocationTarget::ListenServer || To == EFunKTestLocationTarget::DedicatedServer); }
	bool IsClientToClient() const { return (From == EFunKTestLocationTarget::ListenServerClient || From == EFunKTestLocationTarget::DedicatedServerClient) && (To == EFunKTestLocationTarget::ListenServerClient || To == EFunKTestLocationTarget::DedicatedServerClient); }
	bool IsStandalone() const { return From == EFunKTestLocationTarget::Standalone; }

	bool IsFromClient() const { return (From == EFunKTestLocationTarget::ListenServerClient || From == EFunKTestLocationTarget::DedicatedServerClient); }
	bool IsFromServer() const { return (From == EFunKTestLocationTarget::ListenServer || From == EFunKTestLocationTarget::DedicatedServer); }

private:
	EFunKTestLocationTarget From;
	EFunKTestLocationTarget To;

};

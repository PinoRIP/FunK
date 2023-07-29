﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTypes.generated.h"

UENUM(BlueprintType)
enum class EFunKNetMode : uint8
{
	/** Standalone: a game without networking, with one or more local players. Still considered a server because it has all server functionality. */
	Standalone,

	/** Dedicated server: server with no local players. */
	DedicatedServer,

	/** Listen server: a server that also has a local player who is hosting the game, available to other players on the network. */
	ListenServer,

	/**
	 * Network client: client connected to a remote server.
	 * Note that every mode less than this value is a kind of server, so checking NetMode < NM_Client is always some variety of server.
	 */
	Client,

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EFunKClient : uint8
{
	None,

	/** Listen server is also a client */
	Host,
	
	/** First client */
	First,

	/** Second client */
	Second,

	/** Any other client */
	Any,

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EFunKTestEnvironmentType : uint8
{
	Standalone,
	DedicatedServer,
	ListenServer,

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EFunKNetLocation : uint8
{
	Standalone,
	Server,
	Client,

	MAX UMETA(Hidden),
};


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFunKTestLocationTarget: uint8
{
	None					= 0 UMETA(Hidden),
	Standalone				= 1 << 0,
	DedicatedServer			= 1 << 1,
	ListenServer			= 1 << 2,
	DedicatedServerClient	= 1 << 3,
	ListenServerClient		= 1 << 4,
};
ENUM_CLASS_FLAGS(EFunKTestLocationTarget);
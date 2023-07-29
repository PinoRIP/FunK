// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKBlueprintFunctionLibrary.generated.h"

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

/**
 * 
 */
UCLASS()
class FUNK_API UFunKBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static AFunKWorldTestController* GetTestController(UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static int32 GetTestControllerIndex(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetMode GetNetMode(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetMode(UObject* WorldContext, EFunKNetMode& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchClients(UObject* WorldContext, EFunKClient& Branches);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchTestEnvironmentType(UObject* WorldContext, EFunKTestEnvironmentType& Branches);
	
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static bool IsServerDedicated(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetLocation(UObject* WorldContext, EFunKNetLocation& Branches);
};

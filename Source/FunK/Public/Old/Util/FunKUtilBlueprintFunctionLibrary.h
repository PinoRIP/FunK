// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Util/FunKUtilTypes.h"
#include "Old/FunKWorldSubsystem.h"
#include "FunKUtilBlueprintFunctionLibrary.generated.h"



/**
 * 
 */
UCLASS()
class FUNK_API UFunKBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Get the FunK WorldSubsystem
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static UFunKWorldSubsystem* GetFunKWorldSubsystem(const UObject* WorldContext);

	// Gets the network consistent index of this execution. Standalone & Listen/Dedicated server will always be index 0. 
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static int32 GetPeerIndex(const UObject* WorldContext);

	// Current net mode access able in BP
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetMode GetNetMode(const UObject* WorldContext);

	// Switch on the current net mode
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetMode(const UObject* WorldContext, EFunKNetMode& Branches);

	/** Gets the current role:
	 *	ListenSever & Standalone = Host
	 *	The PeerIndex 1 = First
	 *	The PeerIndex 2 = Second
	 *	DedicatedServer = None
	 *	(When there are other clients = Any)
	 */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKClient GetClients(const UObject* WorldContext);

	/** Switch on the current role:
	 *	ListenSever & Standalone = Host
	 *	The PeerIndex 1 = First
	 *	The PeerIndex 2 = Second
	 *	DedicatedServer = None
	 *	(When there are other clients = Any)
	 */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchClients(const UObject* WorldContext, EFunKClient& Branches);

	/** Gets the environment in which the test gets executed (also works on clients) */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKTestEnvironmentType GetTestEnvironmentType(const UObject* WorldContext);

	/** Switches on the environment in which the test gets executed (also works on clients) */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchTestEnvironmentType(const UObject* WorldContext, EFunKTestEnvironmentType& Branches);

	/** Is the server a dedicated server? Works on clients. */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static bool IsServerDedicated(const UObject* WorldContext);

	/** Gets the net location
	 * Standalone = Standalone
	 * Listen/Dedicated server = Server
	 * Client = Client
	 */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext"))
	static EFunKNetLocation GetNetLocation(const UObject* WorldContext);

	/** Switches on the net location
	 * Standalone = Standalone
	 * Listen/Dedicated server = Server
	 * Client = Client
	 */
	UFUNCTION(BlueprintCallable, Category = "FunK|Helper", meta = ( HidePin = "WorldContext", DefaultToSelf = "WorldContext", ExpandEnumAsExecs = "Branches"))
	static void SwitchNetLocation(const UObject* WorldContext, EFunKNetLocation& Branches);
};

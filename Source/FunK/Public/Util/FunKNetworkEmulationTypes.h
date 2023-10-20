// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKNetworkEmulationTypes.generated.h"


UENUM()
enum class EFunKNetworkEmulationTarget
{
	Server UMETA(DisplayName = "Server Only"),
	Client UMETA(DisplayName = "Clients Only"),
	Any UMETA(DisplayName = "Everyone"),
};

USTRUCT()
struct FFunKNetworkEmulationPacketSettings
{
	GENERATED_BODY()

public:
	// Minimum latency to add to packets
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Minimum Latency", ClampMin = "0", ClampMax = "5000"))
	int32 MinLatency = 0;

	// Maximum latency to add to packets. We use a random value between the minimum and maximum (when 0 = always the minimum value)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Maximum Latency", ClampMin = "0", ClampMax = "5000"))
	int32 MaxLatency = 0;

	// Ratio of packets to randomly drop (0 = none, 100 = all)
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))
	int32 PacketLossPercentage = 0;
};

USTRUCT()
struct FFunKCustomNetworkEmulation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(EditCondition = "Profile==Custom"))
	FString Name = FString("Custom");

	// Settings that add latency and packet loss to all outgoing packets
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Outgoing traffic", EditCondition = "Profile==Custom"))
	FFunKNetworkEmulationPacketSettings OutPackets;

	// Settings that add latency and packet loss to all incoming packets
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Incoming traffic", EditCondition = "Profile==Custom"))
	FFunKNetworkEmulationPacketSettings InPackets;
};

USTRUCT()
struct FFunKNetworkEmulation
{
	GENERATED_BODY()

public:
	// Where the emulation should take effect
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Emulation Target"))
	EFunKNetworkEmulationTarget EmulationTarget = EFunKNetworkEmulationTarget::Server;

	// Create custom settings instead of using configured profiles
	UPROPERTY(EditAnywhere)
	bool UseCustomSettings = false;

	// Network emulation profile
	UPROPERTY(EditAnywhere, meta = (GetOptions = "FunK.FunKNetworkEmulationTypesHelper.GetProfileOptions", EditCondition="!UseCustomSettings"))
	FString Profile;

	// Custom settings
	UPROPERTY(EditAnywhere, meta=(EditCondition = "UseCustomSettings"))
	FFunKCustomNetworkEmulation Custom;
};


/**
 * This is just a helper that provides the "GetOptions"-Function used in "FFunKNetworkEmulation".
 */
UCLASS()
class FUNK_API UFunKNetworkEmulationTypesHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "FunK|Setup")
	static TArray<FString> GetProfileOptions();
};

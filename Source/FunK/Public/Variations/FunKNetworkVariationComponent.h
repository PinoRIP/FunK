// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetDriver.h"
#include "Variations/FunKTestVariationComponent.h"
#include "FunKNetworkVariationComponent.generated.h"


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
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Emulation Target"))
	EFunKNetworkEmulationTarget EmulationTarget = EFunKNetworkEmulationTarget::Server;

	UPROPERTY(EditAnywhere)
	bool UseCustomSettings = false;
	
	UPROPERTY(EditAnywhere, meta = (GetOptions = "FunK.FunKPingVariationComponent.GetProfileOptions", EditCondition="!UseCustomSettings"))
	FString Profile;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition = "UseCustomSettings"))
	FFunKCustomNetworkEmulation Custom;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FUNK_API UFunKNetworkVariationComponent : public UFunKTestVariationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFunKNetworkVariationComponent();

	UPROPERTY(EditAnywhere, Category="FunK|Setup")
	TArray<FFunKNetworkEmulation> Emulations;

	virtual int32 GetCount() override;
	virtual void Begin(int32 index) override;
	virtual bool IsReady() override;
	virtual void Finish() override;
	virtual FString GetName() override;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "FunK|Setup")
	static TArray<FString> GetProfileOptions();

private:
	TArray<TKeyValuePair<TWeakObjectPtr<UNetDriver>, FPacketSimulationSettings>> InitialDriverSettings;

};

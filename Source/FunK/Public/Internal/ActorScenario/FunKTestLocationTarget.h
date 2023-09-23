// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKTestLocationTarget.generated.h"

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

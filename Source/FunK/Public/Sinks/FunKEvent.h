// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKEvent.generated.h"

enum class EFunKEventType : uint8
{
	Info,
	Warning,
	Error
};

/**
 * 
 */
USTRUCT()
struct FFunKEvent
{
	GENERATED_BODY()

public:
	FFunKEvent()
		: FFunKEvent(EFunKEventType::Error)
	{
	}
	
	FFunKEvent(EFunKEventType InType)
		: FFunKEvent(InType, FString())
	{
	}
	
	FFunKEvent(EFunKEventType InType, const FString& InMessage)
		: FFunKEvent(InType, InMessage, FString())
	{
	}

	FFunKEvent(EFunKEventType InType, const FString& InMessage, const FString& InContext)
		: Type(InType)
		, Message(InMessage)
		, Context(InContext)
	{
	}

	
	EFunKEventType Type = EFunKEventType::Error;
	FString Message;
	FString Context;

	static FFunKEvent Info(const FString& Message, const FString& Context = "")
	{
		return FFunKEvent(EFunKEventType::Info, Message, Context);
	}

	static FFunKEvent Warning(const FString& Message, const FString& Context = "")
	{
		return FFunKEvent(EFunKEventType::Warning, Message, Context);
	}

	static FFunKEvent Error(const FString& Message, const FString& Context = "")
	{
		return FFunKEvent(EFunKEventType::Error, Message, Context);
	}
};

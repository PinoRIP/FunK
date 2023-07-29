﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEvent.generated.h"

UENUM()
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
	{
		if(!InContext.IsEmpty())
			Context.Add(InContext);
	}

	FFunKEvent(EFunKEventType InType, const FString& InMessage, const TArray<FString>& InContext)
		: Type(InType)
		, Message(InMessage)
		, Context(InContext)
	{
	}

	UPROPERTY()
	EFunKEventType Type = EFunKEventType::Error;

	UPROPERTY()
	FString Message;

	UPROPERTY()
	TArray<FString> Context;

	void GetContext(FString& outContext) const
	{
		outContext = GetContext();
	}

	FORCEINLINE FString GetContext() const
	{
		return FString::Join(Context, TEXT(" | "));
	}

	FFunKEvent& AddToContext(int32 InContext)
	{
		return AddToContext(FString::FromInt(InContext));
	}

	FFunKEvent& AddToContext(const FString& InContext)
	{
		Context.AddUnique(InContext);
		return *this;
	}

	FFunKEvent& AddToContext(const TArray<FString>& InContext)
	{
		for (const FString& String : InContext)
		{
			AddToContext(String);
		}
		return *this;
	}

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

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};
template<>
struct TStructOpsTypeTraits<FFunKEvent> : public TStructOpsTypeTraitsBase2<FFunKEvent>
{
	enum
	{
		WithNetSerializer = true
	};
};

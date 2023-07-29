// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEventBusSubsystem.generated.h"

class AFunKEventBusReplicationController;

typedef TFunction<void(const FFunKEventBusMessage& Message)> FFunKEventBusEventHandler; 

USTRUCT()
struct FFunKEventBusMessage
{
	GENERATED_BODY()

public:
	TSharedPtr<void> Instance;

	UPROPERTY()
	UScriptStruct* Type = nullptr;
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};
template<>
struct TStructOpsTypeTraits<FFunKEventBusMessage> : public TStructOpsTypeTraitsBase2<FFunKEventBusMessage>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT()
struct FUNK_API FReplicationControllerState
{
	GENERATED_BODY()

	UPROPERTY()
	AFunKEventBusReplicationController* Reference = nullptr;

	UPROPERTY()
	bool IsReady = false;
};

USTRUCT()
struct FUNK_API FFunKEventBusControllerEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Controllers = 0;
	
	UPROPERTY()
	int32 ControllersReady = 0;
};

USTRUCT()
struct FUNK_API FFunKEventBusRegistration
{
	GENERATED_BODY()

	FFunKEventBusRegistration() { Key = INDEX_NONE; Subsystem = nullptr; }
	FFunKEventBusRegistration(int32 key, class UFunKEventBusSubsystem* subsystem)
	{
		Key = key;
		Subsystem = subsystem;
	}

public:
	void Unregister();
	bool IsValid() const;

private:
	int32 Key;
	TSoftObjectPtr<class UFunKEventBusSubsystem> Subsystem;

	bool IsBasicValid() const;
};

/**
 * 
 */
UCLASS()
class FUNK_API UFunKEventBusSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	template <typename TStruct>
	FFunKEventBusRegistration On(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	template <typename TStruct>
	FFunKEventBusRegistration At(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	template <typename TStruct>
	bool Has();

	template <typename TStruct>
	void Raise(const TStruct& eventStruct);

	template <typename TStruct>
	const TStruct* Last();

	void RegisterLocalReplicationController(AFunKEventBusReplicationController* localController);

	bool HasLocalReplicationController() const;
	
	void ReplicationControllerReady(AFunKEventBusReplicationController* controller);

	void ReceiveMessage(const FFunKEventBusMessage& Message);

	AFunKEventBusReplicationController* GetLocalController() const { return LocalController; }
	
private:
	UPROPERTY()
	TArray<FReplicationControllerState> ReplicationControllers;
	TMap<int32, FFunKEventBusEventHandler> Handlers;

	UPROPERTY()
	TMap<FString, FFunKEventBusMessage> LastEvents;

	UPROPERTY()
	AFunKEventBusReplicationController* LocalController = nullptr;

	int32 HandlerCounter = 0;

	template <typename TStruct>
	int32 AddHandler(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	template <typename TStruct>
	const FFunKEventBusMessage* GetLast();
	
	void OnConnect(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnDisconnect(AGameModeBase* GameMode, AController* Controller);
	void UpdateControllerStats();

	void RegisterController(APlayerController* NewPlayer);
	void DeregisterController(AController* Controller);

	void Unregister(int32 key);
	bool HasHandler(int32 key);

	void SendMessage(const FFunKEventBusMessage& Message);

	
	friend FFunKEventBusRegistration;
};

template <typename TStruct>
FFunKEventBusRegistration UFunKEventBusSubsystem::On(TFunction<void(const TStruct& Struct)> ReceivingLambda)
{
	const int32 key = AddHandler(ReceivingLambda);
	return FFunKEventBusRegistration(key, this);
}

template <typename TStruct>
FFunKEventBusRegistration UFunKEventBusSubsystem::At(TFunction<void(const TStruct& Struct)> ReceivingLambda)
{
	const int32 key = AddHandler(ReceivingLambda);

	if(const FFunKEventBusMessage* lastEventMessage = GetLast<TStruct>())
	{
		Handlers[key](*lastEventMessage);
	}
	
	return FFunKEventBusRegistration(key, this);
}

template <typename TStruct>
bool UFunKEventBusSubsystem::Has()
{
	return !!GetLast<TStruct>();
}

template <typename TStruct>
void UFunKEventBusSubsystem::Raise(const TStruct& eventStruct)
{
	FFunKEventBusMessage message;
	message.Type = TStruct::StaticStruct();
	message.Instance = TSharedPtr<void>(new TStruct(eventStruct));

	SendMessage(message);
}

template <typename TStruct>
const TStruct* UFunKEventBusSubsystem::Last()
{
	if(const FFunKEventBusMessage* lastEventMessage = GetLast<TStruct>())
	{
		return static_cast<TStruct*>(lastEventMessage->Instance.Get());
	}

	return nullptr;
}

template <typename TStruct>
int32 UFunKEventBusSubsystem::AddHandler(TFunction<void(const TStruct& Struct)> ReceivingLambda)
{
	int32 key = this->HandlerCounter++;
	Handlers.Add(key, [ReceivingLambda, this](const FFunKEventBusMessage& message)
	{
		if(message.Type == TStruct::StaticStruct())
		{
			TStruct* castedInstance = static_cast<TStruct*>(message.Instance.Get());
			ReceivingLambda(*castedInstance);
		}
	});

	return key;
}

template <typename TStruct>
const FFunKEventBusMessage* UFunKEventBusSubsystem::GetLast()
{
	const FString structName = TStruct::StaticStruct()->GetName();
	return LastEvents.Find(structName);
}
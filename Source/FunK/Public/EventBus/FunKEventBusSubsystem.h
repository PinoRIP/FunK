// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEventBusSubsystem.generated.h"

struct FFunKEventBusMessage;
class AFunKEventBusReplicationController;

typedef TFunction<void(const FFunKEventBusMessage& Message)> FFunKEventBusEventHandler; 

USTRUCT()
struct FFunKEventBusMessage
{
	GENERATED_BODY()

public:
	FGuid CallbackId;

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

USTRUCT()
struct FUNK_API FFunKEventCallbackState
{
	GENERATED_BODY()

public:
	FGuid Id;
	
	UPROPERTY()
	TArray<AFunKEventBusReplicationController*> DispatchedControllers;
	
	TFunction<void()> Function;
};

/**
 * Eventbus system to send events from anywhere to anywhere.
 * An event triggered an client A will be received on the Server, Client A & Client B.
 * Events are structs that are differentiated by the struct type.
 */
UCLASS()
class FUNK_API UFunKEventBusSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// The lambda will be triggered when the struct is received.
	template <typename TStruct>
	FFunKEventBusRegistration On(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	// The lambda will be triggered when the struct is received. When an event of this type has already been received the lambda will be invoked instantly with the last received version.
	template <typename TStruct>
	FFunKEventBusRegistration At(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	// The system has already received an event of this type.
	template <typename TStruct>
	bool Has();

	// Raises the event
	template <typename TStruct>
	void Raise(const TStruct& eventStruct);

	// Raises the event. After all receiving lambdas have been executed the callback lambda will be called.
	template <typename TStruct>
	void Raise(const TStruct& eventStruct, TFunction<void()> callback);

	// Gets the last version of a given event.
	template <typename TStruct>
	const TStruct* Last();

	// Returns true when the event bus system has any registered handler lambdas
	bool AnyHandler() const;

	// Returns true when the event bus owns a local replication controller
	bool HasLocalReplicationController() const;

	// Gets the local replication controller
	AFunKEventBusReplicationController* GetLocalController() const { return LocalController; }

	// Gets the amount of existing replication controllers (every peer except the server has one).
	int32 GetReplicationControllerCount() const;

	// Gets the amount of replication controllers that are ready to relay events.
	int32 GetReadyReplicationControllerCount() const;

	// Gets all replication controllers (sever only)
	void GetReplicationControllers(TArray<AFunKEventBusReplicationController*>& Array);
	
private:
	UPROPERTY()
	TArray<FReplicationControllerState> ReplicationControllers;
	TMap<int32, FFunKEventBusEventHandler> Handlers;

	UPROPERTY()
	TMap<FString, FFunKEventBusMessage> LastEvents;
	
	UPROPERTY()
	TMap<FGuid, FFunKEventCallbackState> Callbacks;

	UPROPERTY()
	AFunKEventBusReplicationController* LocalController = nullptr;

	int32 HandlerCounter = 0;
	int32 EventCallbackCounter = 0;

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

	void SendMessage(FFunKEventBusMessage& Message, TOptional<TFunction<void()>>& Callback);

	void RegisterLocalReplicationController(AFunKEventBusReplicationController* localController);
	void ReplicationControllerReady(AFunKEventBusReplicationController* controller);
	void ReceiveMessage(const FFunKEventBusMessage& Message);

	void CheckCallback(FGuid callbackId, AFunKEventBusReplicationController* FromController);

	template <typename TStruct>
	void RaiseImpl(const TStruct& eventStruct, TOptional<TFunction<void()>> Callback);

	friend AFunKEventBusReplicationController;
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
	RaiseImpl<TStruct>(eventStruct, NullOpt);
}

template <typename TStruct>
void UFunKEventBusSubsystem::Raise(const TStruct& eventStruct, TFunction<void()> callback)
{
	RaiseImpl<TStruct>(eventStruct, callback);
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

template <typename TStruct>
void UFunKEventBusSubsystem::RaiseImpl(const TStruct& eventStruct, TOptional<TFunction<void()>> Callback)
{
	FFunKEventBusMessage message;
	message.Type = TStruct::StaticStruct();
	message.Instance = TSharedPtr<void>(new TStruct(eventStruct));

	SendMessage(message, Callback);
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKEventBusSubsystem.generated.h"

class AFunKEventBusReplicationController;

USTRUCT()
struct FFunKEventBusMessage
{
	GENERATED_BODY()

public:
	TSharedPtr<void> Instance;
	UScriptStruct* Type;
	
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
struct FEventBusRegistration
{
	GENERATED_BODY()

	FEventBusRegistration() { Index = INDEX_NONE; Subsystem = nullptr; }
	FEventBusRegistration(int32 index, class UFunKEventBusSubsystem* subsystem)
	{
		Index = index;
		Subsystem = subsystem;
	}

public:
	void Unregister() const;

private:
	int32 Index;
	TSoftObjectPtr<class UFunKEventBusSubsystem> Subsystem;
};

USTRUCT()
struct FReplicationControllerState
{
	GENERATED_BODY()

	UPROPERTY()
	AFunKEventBusReplicationController* Reference = nullptr;

	UPROPERTY()
	bool IsReady = false;
};

USTRUCT()
struct FEventBusControllerEvent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Controllers = 0;
	
	UPROPERTY()
	int32 ControllersReady = 0;
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
	FEventBusRegistration Register(TFunction<void(const TStruct& Struct)> ReceivingLambda);

	template <typename TStruct>
	void Raise(const TStruct& eventStruct);

	void RegisterLocalReplicationController(AFunKEventBusReplicationController* localController);

	bool HasLocalReplicationController() const;
	
	void ReplicationControllerReady(AFunKEventBusReplicationController* controller);

	void ReceiveMessage(const FFunKEventBusMessage& Message);
	
private:
	UPROPERTY()
	TArray<FReplicationControllerState> ReplicationControllers;
	TArray<TFunction<void(const FFunKEventBusMessage& Message)>> Handlers;

	UPROPERTY()
	AFunKEventBusReplicationController* LocalController = nullptr;
	
	void OnConnect(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnDisconnect(AGameModeBase* GameMode, AController* Controller);
	void UpdateControllerStats();

	void RegisterController(APlayerController* NewPlayer);
	void DeregisterController(AController* Controller);

	void Unregister(int32 index);

	void SendMessage(const FFunKEventBusMessage& Message);

	
	friend FEventBusRegistration;
};

template <typename TStruct>
FEventBusRegistration UFunKEventBusSubsystem::Register(TFunction<void(const TStruct& Struct)> ReceivingLambda)
{
	const int32 index = Handlers.Add([ReceivingLambda, this](const FFunKEventBusMessage& message)
	{
		if(message.Type == TStruct::StaticStruct())
		{
			TStruct* castedInstance = static_cast<TStruct*>(message.Instance.Get());
			ReceivingLambda(*castedInstance);
		}
	});

	return FEventBusRegistration(index, this);
}

template <typename TStruct>
void UFunKEventBusSubsystem::Raise(const TStruct& eventStruct)
{
	FFunKEventBusMessage message;
	message.Type = TStruct::StaticStruct();
	message.Instance = TSharedPtr<void>(new TStruct(eventStruct));

	SendMessage(message);
}
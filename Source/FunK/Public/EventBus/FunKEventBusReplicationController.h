// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKEventBusReplicationController.generated.h"

struct FFunKEventBusMessage;
class AFunKEventBusReplicationController;

USTRUCT()
struct FFunKCallbackProxyState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<AFunKEventBusReplicationController*> Controllers;
};

/*
 * Controller that is owned by the player controllers and relays events for the EventBusSubsystem
 */
UCLASS()
class FUNK_API AFunKEventBusReplicationController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunKEventBusReplicationController();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	// Gets the amount of controllers ready to relay events
	int32 GetActiveController() const { return ActiveController; }

	// Gets the amount of controllers
	int32 GetControllerNumber() const { return ControllerNumber; }

	// Returns true given the server is a dedicated server (works on clients)
	bool GetIsServerDedicated() const { return IsServerDedicated; }
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnRep_Owner() override;

private:
	bool IsControllerLocallyReady() const;

	UFUNCTION(Server, Reliable)
	void ServerSendMessage(const FFunKEventBusMessage& Message);

	UFUNCTION(Client, Reliable)
	void ClientSendMessage(const FFunKEventBusMessage& Message);

	UFUNCTION(Server, Reliable)
	void ServerEventCallback(FGuid CallbackId);

	UFUNCTION(Client, Reliable)
	void ClientEventCallback(FGuid CallbackId);

	UFUNCTION(Server, Reliable)
	void ServerControllerReady();
	
	void NotifyReplicationControllerRemoved(AFunKEventBusReplicationController* ReplicationController);

	bool ReportProxiedCallbackResponse(AFunKEventBusReplicationController* ReplicationController, FGuid CallbackId);
	
private:
	UPROPERTY( replicated )
	int32 ActiveController;

	UPROPERTY( replicated )
	int32 ControllerNumber = INDEX_NONE;
	
	UPROPERTY( replicated )
	bool IsServerDedicated = false;

	UPROPERTY()
	TMap<FGuid, FFunKCallbackProxyState> ProxiedCallbacks;

	bool IsControllerReadinessSend = false;
	
	void CheckControllerReady();

	friend class UFunKEventBusSubsystem;
};

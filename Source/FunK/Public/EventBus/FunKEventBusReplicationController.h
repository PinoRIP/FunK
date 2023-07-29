// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunKEventBusReplicationController.generated.h"

struct FFunKEventBusMessage;

UCLASS()
class FUNK_API AFunKEventBusReplicationController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunKEventBusReplicationController();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	int32 GetActiveController() const { return ActiveController; }
	int32 GetControllerNumber() const { return ControllerNumber; }
	int32 GetIsServerDedicated() const { return IsServerDedicated; }
	
protected:
	bool IsControllerLocallyReady() const;

	void CheckControllerReady();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSendMessage(const FFunKEventBusMessage& message);

	UFUNCTION(Client, Reliable)
	void ClientSendMessage(const FFunKEventBusMessage& message);

	UFUNCTION(Server, Reliable)
	void ServerControllerReady();

	virtual void OnRep_Owner() override;

private:
	UPROPERTY( replicated )
	int32 ActiveController;

	UPROPERTY( replicated )
	int32 ControllerNumber = INDEX_NONE;
	
	UPROPERTY( replicated )
	bool IsServerDedicated = false;

	bool IsControllerReadinessSend = false;

	friend class UFunKEventBusSubsystem;
};

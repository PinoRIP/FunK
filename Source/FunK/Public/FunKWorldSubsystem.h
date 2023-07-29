// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "FunKWorldSubsystem.generated.h"

class AFunKWorldTestController;
/**
 * 
 */
UCLASS()
class FUNK_API UFunKWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	AFunKWorldTestController* GetLocalTestController();
	void SetLocalTestController(AFunKWorldTestController* localTestController);

	bool HasLocalTestController() const;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void Deinitialize() override;

	int32 GetPeerIndex() const;
	int32 GetPeerCount() const;
	bool IsServerDedicated() const;
	
private:
	UPROPERTY()
	AFunKWorldTestController* LocalTestController = nullptr;

	AFunKWorldTestController* NewTestController() const;

	FFunKEventBusRegistration Registration;
};

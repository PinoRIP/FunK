// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void CheckLocalTestController();
	AFunKWorldTestController* GetLocalTestController();
	void SetLocalTestController(AFunKWorldTestController* localTestController);

	bool HasLocalTestController() const;
	
private:
	UPROPERTY()
	AFunKWorldTestController* LocalTestController = nullptr;

	AFunKWorldTestController* NewTestController() const;
};

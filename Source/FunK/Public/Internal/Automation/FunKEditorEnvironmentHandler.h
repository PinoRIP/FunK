// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKEnvironmentHandler.h"
#include "Internal/EventBus/FunKEventBusSubsystem.h"
#include "UObject/Object.h"
#include "FunKEditorEnvironmentHandler.generated.h"

/**
 * 
 */
UCLASS()
class FUNK_API UFunKEditorEnvironmentHandler : public UObject, public IFunKEnvironmentHandler
{
	GENERATED_BODY()

public:
	virtual EFunKEnvironmentWorldState UpdateWorldState(const FFunKTestInstructions& Instructions) override;
	
	virtual UWorld* GetWorld() override;

private:
	TArray<uint32> StartedProcesses;
	FFunKEventBusRegistration EventBusRegistration;
	bool WaitsForWorld = false;
	bool WaitsForControllers = false;
	
	UPROPERTY()
	UWorld* World;
	
	bool IsEnvironmentRunning(const FFunKTestInstructions& Instructions, bool& isWrongEnvironmentRunning);
	FString GetCurrentPieWorldPackageName();
	const FWorldContext* GetCurrentPieWorldContext();
	bool IsHoldingSubprocesses() const;

	virtual bool StartEnvironment(const FFunKTestInstructions& Instructions);
	static void SetFpsSettings(class ULevelEditorPlaySettings* playSettings, const struct FFunKSettings& funkSettings, bool isDedicated);

	static int32 GetTargetReplicationControllerCount(const FFunKTestInstructions& Instructions);
};

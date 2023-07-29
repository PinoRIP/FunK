// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunKTestInstructions.h"
#include "Misc/AutomationTest.h"
#include "UObject/Object.h"
#include "FunKAutomationLatentWaitForTestRunCommand.generated.h"

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FFunKAutomationLatentWaitForTestRunCommand, TWeakObjectPtr<UFunKEngineSubsystem>, EngineSubsystem, FFunKTestInstructions, TestInstructions);


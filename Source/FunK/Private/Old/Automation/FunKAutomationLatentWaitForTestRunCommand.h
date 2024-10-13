// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Old/Automation/FunKTestInstructions.h"
#include "Misc/AutomationTest.h"
#include "UObject/Object.h"

class UFunKEngineSubsystem;

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FFunKAutomationLatentWaitForTestRunCommand, TWeakObjectPtr<UFunKEngineSubsystem>, EngineSubsystem, FFunKTestInstructions, TestInstructions);


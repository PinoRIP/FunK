#pragma once
#include "FunKTestRunner.h"
#include "Misc/AutomationTest.h"

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FFunKAutomationLatentTestRunCommand, TWeakObjectPtr<UFunKTestRunner>, TestRun, FFunKTestInstructions, TestInstructions);

#pragma once
#include "FunKTestRunner.h"
#include "Misc/AutomationTest.h"

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FFunKAutomationLatentTestRunCommand, TWeakObjectPtr<UFunKTestRunner>, TestRun);

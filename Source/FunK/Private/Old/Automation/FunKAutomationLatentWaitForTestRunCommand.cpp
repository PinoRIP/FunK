// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Automation/FunKAutomationLatentWaitForTestRunCommand.h"
#include "Old/Automation/FunKEngineSubsystem.h"
#include "Old/Automation/FunKAutomationEntry.h"

bool FFunKAutomationLatentWaitForTestRunCommand::Update()
{
	if (EngineSubsystem.IsValid())
	{
		if (EngineSubsystem->IsRunning())
			return false;
		
		FFunKAutomationEntry::StartTest(EngineSubsystem.Get(), TestInstructions);
	}

	return true;
}

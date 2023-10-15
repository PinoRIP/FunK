// Fill out your copyright notice in the Description page of Project Settings.


#include "Automation/FunKAutomationLatentWaitForTestRunCommand.h"
#include "Automation/FunKEngineSubsystem.h"
#include "Automation/FunKAutomationEntry.h"

bool FFunKAutomationLatentWaitForTestRunCommand::Update()
{
	if(EngineSubsystem.IsValid())
	{
		if(EngineSubsystem->IsRunning()) return false;
		FFunKAutomationEntry::StartTest(EngineSubsystem.Get(), TestInstructions);
	}

	return true;
}

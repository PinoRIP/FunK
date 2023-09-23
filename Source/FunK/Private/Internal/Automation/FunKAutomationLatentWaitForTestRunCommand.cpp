// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/Automation/FunKAutomationLatentWaitForTestRunCommand.h"
#include "Internal/Automation/FunKEngineSubsystem.h"
#include "Internal/Automation/FunKAutomationEntry.h"

bool FFunKAutomationLatentWaitForTestRunCommand::Update()
{
	if(EngineSubsystem.IsValid())
	{
		if(EngineSubsystem->IsRunning()) return false;
		FFunKAutomationEntry::StartTest(EngineSubsystem.Get(), TestInstructions);
	}

	return true;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Sinks/FunKInProcAutomationSink.h"

#include "FunKAutomationEntry.h"
#include "FunKTestRunner.h"

void UFunKInProcAutomationSink::Init(UFunKTestRunner* run)
{
	AutomationEntryRuntime = GetCurrentAutomationEntry();
}

void UFunKInProcAutomationSink::RaiseEvent(const FFunKEvent& raisedEvent, const UFunKTestRunner* run)
{
	if(AutomationEntryRuntime)
	{
		AutomationEntryRuntime->AddEvent(FAutomationEvent(GetAutomationEventType(raisedEvent.Type), raisedEvent.Message, raisedEvent.Context));
	}
}

EAutomationEventType UFunKInProcAutomationSink::GetAutomationEventType(EFunKEventType eventType)
{
	switch (eventType)
	{
	case EFunKEventType::Info: return EAutomationEventType::Info;
	case EFunKEventType::Warning: return EAutomationEventType::Warning;
	case EFunKEventType::Error: return EAutomationEventType::Error;
	default: return EAutomationEventType::Error;
	}
}

bool UFunKInProcAutomationSink::IsAvailable()
{
	return GetCurrentAutomationEntry() != nullptr;
}

FFunKAutomationEntry* UFunKInProcAutomationSink::GetCurrentAutomationEntry()
{
	return static_cast<FFunKAutomationEntry*>(FAutomationTestFramework::Get().GetCurrentTest());
}

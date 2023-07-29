// Fill out your copyright notice in the Description page of Project Settings.


#include "Sinks/FunKAutomationSink.h"
#include "Automation/FunKAutomationEntry.h"

void UFunKAutomationSink::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(FFunKAutomationEntry* AutomationEntryRuntime = GetCurrentAutomationEntry())
	{
		AutomationEntryRuntime->AddEvent(FAutomationEvent(GetAutomationEventType(raisedEvent.Type), raisedEvent.Message, raisedEvent.GetContext()));
	}
}

bool UFunKAutomationSink::IsAvailable()
{
	return GetCurrentAutomationEntry() != nullptr;
}

EAutomationEventType UFunKAutomationSink::GetAutomationEventType(EFunKEventType eventType)
{
	switch (eventType)
	{
	case EFunKEventType::Info: return EAutomationEventType::Info;
	case EFunKEventType::Warning: return EAutomationEventType::Warning;
	case EFunKEventType::Error: return EAutomationEventType::Error;
	default: return EAutomationEventType::Error;
	}
}

FFunKAutomationEntry* UFunKAutomationSink::GetCurrentAutomationEntry()
{
	return static_cast<FFunKAutomationEntry*>(FAutomationTestFramework::Get().GetCurrentTest());
}

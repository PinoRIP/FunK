// Fill out your copyright notice in the Description page of Project Settings.

#include "Sinks/FunKLogSink.h"
#include "FunKLogging.h"

void UFunKLogSink::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	LogEvent(raisedEvent);
}

void UFunKLogSink::LogEvent(const FFunKEvent& raisedEvent)
{
	switch (raisedEvent.Type)
	{
	case EFunKEventType::Info: UE_LOG(FunKLog, Log, TEXT("Raised %s in Context %s"), *raisedEvent.Message, *raisedEvent.GetContext()) break;
	case EFunKEventType::Warning: UE_LOG(FunKLog, Warning, TEXT("Raised %s in Context %s"), *raisedEvent.Message, *raisedEvent.GetContext()) break;
	case EFunKEventType::Error: UE_LOG(FunKLog, Error, TEXT("Raised %s in Context %s"), *raisedEvent.Message, *raisedEvent.GetContext()) break;
	default: UE_LOG(FunKLog, Error, TEXT("Unknown event type Raised %s in Context %s"), *raisedEvent.Message, *raisedEvent.GetContext()) ;
	}
}

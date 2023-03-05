// Fill out your copyright notice in the Description page of Project Settings.


#include "Sinks/FunKExtAutomationSink.h"

#include "FunKLogging.h"
#include "FunKTestRunner.h"
#include "FunKWorldTestController.h"

void UFunKExtAutomationSink::RaiseEvent(const FFunKEvent& raisedEvent, const UFunKTestRunner* run)
{
	if(const AFunKWorldTestController* controller = run->GetCurrentWorldController())
	{
		controller->SendEvent(raisedEvent.Type, raisedEvent.Message, raisedEvent.Context);
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("Event could not be send: Missing world controller for RPC communication!"))
	}
}

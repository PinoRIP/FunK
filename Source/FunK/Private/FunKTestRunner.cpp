// Fill out your copyright notice in the Description page of Project Settings.
#include "FunKTestRunner.h"
#include "FileHelpers.h"
#include "FunK.h"
#include "FunKEngineSubsystem.h"
#include "Captures/FunKFailedPieStartCapture.h"
#include "Captures/FunKNewProcessCapture.h"
#include "FunKSettingsObject.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "Editor/EditorPerformanceSettings.h"
#include "Environment/FunKEditorEnvironmentHandler.h"
#include "Events/FunKEvent.h"
#include "GameFramework/GameStateBase.h"
#include "Misc/AutomationTest.h"
#include "UObject/UnrealTypePrivate.h"

class UFunKSettingsObject;

void UFunKTestRunner::Init(UFunKEngineSubsystem* funKEngineSubsystem, EFunKTestRunnerType RunType)
{
	Type = RunType;
	FunKEngineSubsystem = funKEngineSubsystem;
	EnvironmentHandler = RunType == EFunKTestRunnerType::LocalInProc
		? NewObject<UFunKEditorEnvironmentHandler>()
		: nullptr;

	check(EnvironmentHandler);
	UpdateState(EFunKTestRunnerState::Initialized);
}

bool UFunKTestRunner::Start(const FFunKTestInstructions& Instructions)
{	
	UpdateState(Type > EFunKTestRunnerType::LocalInProc ? EFunKTestRunnerState::ExecutingTest : EFunKTestRunnerState::Started);
	RaiseInfoEvent("FunK Start");
	ActiveTestInstructions = Instructions;	
	return true;
}

bool UFunKTestRunner::Update()
{
	if(Type != EFunKTestRunnerType::LocalInProc)
	{
		RaiseErrorEvent("The tests can currently only be started via. Editor!", "UFunKTestRunner::Update");
		return true;
	}
	
	if(State != EFunKTestRunnerState::ExecutingTest)
	{
		if(State <= EFunKTestRunnerState::Ended)
		{
			RaiseErrorEvent("The test run is not active!", "UFunKTestRunner::Update");
			return true;
		}

		const EFunKEnvironmentWorldState environmentState = EnvironmentHandler->UpdateWorldState(ActiveTestInstructions);
		if(environmentState == EFunKEnvironmentWorldState::CantStart)
		{
			RaiseErrorEvent("Test environment could not be setup", "UFunKTestRunner::Update");
			End();
			return true;
		}
		
		UpdateState(environmentState == EFunKEnvironmentWorldState::IsRunning ? EFunKTestRunnerState::Ready : EFunKTestRunnerState::EnvironmentSetup);
		
		if(State == EFunKTestRunnerState::Ready)
		{
			EventBusRegistration.Add(EnvironmentHandler->GetEventBus()->On<FFunKEvent>([this](const FFunKEvent& Event)
			{
				RaiseEvent(Event);
			}));
			
			if(ActiveTestInstructions.MapTestName.Len() > 0)
			{
				GetCurrentWorldController()->ExecuteTestByName(ActiveTestInstructions.MapTestName);
			}
			else
			{
				GetCurrentWorldController()->ExecuteAllTests();
			}
			
			UpdateState(EFunKTestRunnerState::ExecutingTest);
		}
	}
	else
	{
		const AFunKWorldTestController* worldController = GetCurrentWorldController();
		if(!worldController)
		{
			RaiseWarningEvent("Canceled test run: world is tearing down!", "UFunKTestRunner::Update");
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
		else if(worldController->IsFinished())
		{
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
	}

	if(State == EFunKTestRunnerState::EvaluatingTest)
	{
		End();
		return true;
	}

	return false;
}

void UFunKTestRunner::End()
{
	EventBusRegistration.Unregister();
	UpdateState(EFunKTestRunnerState::Ended);
}

void UFunKTestRunner::RaiseInfoEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Info(Message, Context));
}

void UFunKTestRunner::RaiseWarningEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Warning(Message, Context));
}

void UFunKTestRunner::RaiseErrorEvent(const FString& Message, const FString& Context) const
{
	RaiseEvent(FFunKEvent::Error(Message, Context));
}

void UFunKTestRunner::RaiseEvent(const FFunKEvent& raisedEvent) const
{
	if(auto* AutomationEntryRuntime = FAutomationTestFramework::Get().GetCurrentTest())
	{	
		EAutomationEventType type = raisedEvent.Type == EFunKEventType::Info
			? EAutomationEventType::Info
			: raisedEvent.Type == EFunKEventType::Warning
				? EAutomationEventType::Warning
				: EAutomationEventType::Error;
		
		AutomationEntryRuntime->AddEvent(FAutomationEvent(type, raisedEvent.Message, raisedEvent.GetContext()));
	}
}

bool UFunKTestRunner::IsRunning() const
{
	return State >= EFunKTestRunnerState::Started;
}

UFunKEngineSubsystem* UFunKTestRunner::GetSubsystem() const
{
	return FunKEngineSubsystem.Get();
}

AFunKWorldTestController* UFunKTestRunner::GetCurrentWorldController() const
{
	return EnvironmentHandler->GetTestController();
}

void UFunKTestRunner::UpdateState(EFunKTestRunnerState newState)
{
	State = newState;
}

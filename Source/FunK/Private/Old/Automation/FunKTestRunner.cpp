// Fill out your copyright notice in the Description page of Project Settings.
#include "Old/Automation/FunKTestRunner.h"
#include "Old/Automation/FunKEngineSubsystem.h"
#include "Old/FunKWorldTestController.h"
#include "Old/Automation/FunKEditorEnvironmentHandler.h"
#include "Old/Events/FunKEvent.h"
#include "Misc/AutomationTest.h"

class UFunKSettingsObject;

void UFunKTestRunner::Init(UFunKEngineSubsystem* InFunKEngineSubsystem, const EFunKTestRunnerType InType)
{
	Type = InType;
	FunKEngineSubsystem = InFunKEngineSubsystem;
	EnvironmentHandler = InType == EFunKTestRunnerType::LocalInProc
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
	if (Type != EFunKTestRunnerType::LocalInProc)
	{
		RaiseErrorEvent("The tests can currently only be started via. Editor!", "UFunKTestRunner::Update");
		return true;
	}
	
	if (State != EFunKTestRunnerState::ExecutingTest)
	{
		if (State <= EFunKTestRunnerState::Ended)
		{
			RaiseErrorEvent("The test run is not active!", "UFunKTestRunner::Update");
			return true;
		}

		const EFunKEnvironmentWorldState EnvironmentState = EnvironmentHandler->UpdateWorldState(ActiveTestInstructions);
		if (EnvironmentState == EFunKEnvironmentWorldState::CantStart)
		{
			RaiseErrorEvent("Test environment could not be setup", "UFunKTestRunner::Update");
			End();
			return true;
		}
		
		UpdateState(EnvironmentState == EFunKEnvironmentWorldState::IsRunning ? EFunKTestRunnerState::Ready : EFunKTestRunnerState::EnvironmentSetup);
		
		if (State == EFunKTestRunnerState::Ready)
		{
			EventBusRegistration.Add(EnvironmentHandler->GetEventBus()->On<FFunKEvent>([this](const FFunKEvent& Event)
			{
				RaiseEvent(Event);
			}));
			
			if (ActiveTestInstructions.MapTestName.Len() > 0)
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
		const AFunKWorldTestController* WorldController = GetCurrentWorldController();
		if (!WorldController)
		{
			RaiseWarningEvent("Canceled test run: world is tearing down!", "UFunKTestRunner::Update");
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
		else if (WorldController->IsFinished())
		{
			UpdateState(EFunKTestRunnerState::EvaluatingTest);
		}
	}

	if (State == EFunKTestRunnerState::EvaluatingTest)
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

void UFunKTestRunner::RaiseEvent(const FFunKEvent& RaisedEvent) const
{
	if (auto* AutomationEntryRuntime = FAutomationTestFramework::Get().GetCurrentTest())
	{	
		const EAutomationEventType EventType = RaisedEvent.Type == EFunKEventType::Info
			? EAutomationEventType::Info
			: RaisedEvent.Type == EFunKEventType::Warning
				? EAutomationEventType::Warning
				: EAutomationEventType::Error;
		
		AutomationEntryRuntime->AddEvent(FAutomationEvent(EventType, RaisedEvent.Message, RaisedEvent.GetContext()));
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

void UFunKTestRunner::UpdateState(const EFunKTestRunnerState NewState)
{
	State = NewState;
}

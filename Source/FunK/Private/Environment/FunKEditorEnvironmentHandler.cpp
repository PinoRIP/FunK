// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/FunKEditorEnvironmentHandler.h"

#include "FunK.h"
#include "FunKSettingsObject.h"
#include "FunKTestInstructions.h"
#include "Captures/FunKFailedPieStartCapture.h"
#include "Captures/FunKNewProcessCapture.h"
#include "Editor/EditorPerformanceSettings.h"

class UEditorPerformanceSettings;
class UFunKSettingsObject;

EFunKEnvironmentWorldState UFunKEditorEnvironmentHandler::UpdateWorldState(const FFunKTestInstructions& Instructions)
{
	bool isWrongEnvironment = false;
	const bool isEnvironmentRunning = IsEnvironmentRunning(Instructions, isWrongEnvironment);
	if(!isEnvironmentRunning || isWrongEnvironment)
	{
		if(!WaitsForWorld)
		{
			if(isWrongEnvironment)
			{
				if(isEnvironmentRunning)
					GEditor->EndPlayMap();
				
				GEditor->EndPlayOnLocalPc();
				StartedProcesses.Empty(StartedProcesses.Num());
			}
		
			if(!StartEnvironment(Instructions))
			{
				return EFunKEnvironmentWorldState::CantStart;
			}
		
			WaitsForWorld = true;
		}
	}
	else
	{
		if(WaitsForWorld)
		{
			if(!World) World = GetCurrentPieWorldContext()->World();
            WaitsForWorld = !(World && World->HasBegunPlay());
			WaitsForControllers = !WaitsForWorld;
		}

		if(WaitsForControllers)
		{
			WaitsForControllers = GetEventBus()->GetReadyReplicationControllerCount() != GetTargetReplicationControllerCount(Instructions);
		}
	}
	
	return WaitsForWorld || WaitsForControllers
		? EFunKEnvironmentWorldState::IsStarting
		: EFunKEnvironmentWorldState::IsRunning;
}

UWorld* UFunKEditorEnvironmentHandler::GetWorld()
{
	return World;
}

bool UFunKEditorEnvironmentHandler::IsEnvironmentRunning(const FFunKTestInstructions& Instructions, bool& isWrongEnvironmentRunning)
{
	isWrongEnvironmentRunning = false;

	const FFunKSettings& settings = GetDefault<UFunKSettingsObject>()->Settings;
	const FString currentPieWorldPackageName = GetCurrentPieWorldPackageName();
	const bool isNoLocalMapRunning = (currentPieWorldPackageName.IsEmpty() || currentPieWorldPackageName.Len() <= 0);
	const bool isLocalProcessOnly = settings.RunTestUnderOneProcess || Instructions.IsStandaloneTest();
	if(isNoLocalMapRunning)
	{
		isWrongEnvironmentRunning = !isLocalProcessOnly && IsHoldingSubprocesses();
		return false;
	}

	isWrongEnvironmentRunning = currentPieWorldPackageName != Instructions.MapPackageName;
	if(isWrongEnvironmentRunning || isLocalProcessOnly)
		return true;

	for (const uint32 StartedProcess : StartedProcesses)
	{
		if(!FPlatformProcess::IsApplicationRunning(StartedProcess))
		{
			isWrongEnvironmentRunning = true;
			break;
		}
	}

	return true;
}

FString UFunKEditorEnvironmentHandler::GetCurrentPieWorldPackageName()
{
	const FWorldContext* WorldContext = GetCurrentPieWorldContext();
	if(!WorldContext) return "";

	const FString WorldPackage = WorldContext->World()->GetOutermost()->GetName();
	return UWorld::StripPIEPrefixFromPackageName(WorldPackage, WorldContext->World()->StreamingLevelsPrefix);
}

const FWorldContext* UFunKEditorEnvironmentHandler::GetCurrentPieWorldContext()
{
	//TODO: do we want to support multiple worlds in parallel?
	const TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();
	for (auto& Context : WorldContexts)
	{
		if (Context.World())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				return &Context;
			}
		}
	}

	return nullptr;
}

bool UFunKEditorEnvironmentHandler::IsHoldingSubprocesses() const
{
	return StartedProcesses.Num() > 0;
}

bool UFunKEditorEnvironmentHandler::StartEnvironment(const FFunKTestInstructions& Instructions)
{
	FFunKFailedPieStartCapture FailHandler;
	FFunKNewProcessCapture ProcessStartCapture;

	UEditorPerformanceSettings* Settings = GetMutableDefault<UEditorPerformanceSettings>();
	Settings->bThrottleCPUWhenNotForeground = false;
	Settings->bMonitorEditorPerformance = false;
	Settings->PostEditChange();
		
	FRequestPlaySessionParams params;
	params.EditorPlaySettings = NewObject<ULevelEditorPlaySettings>();
	params.EditorPlaySettings->NewWindowHeight = 1080;
	params.EditorPlaySettings->NewWindowWidth = 1920;
	
	params.GlobalMapOverride = Instructions.MapPackageName;
	params.AdditionalStandaloneCommandLineParameters = FFunKModule::FunkTestStartParameter;
	
	// GEditor->EndPlayMap();

	FProperty* Property = ULevelEditorPlaySettings::StaticClass()->FindPropertyByName(FName("ServerMapNameOverride"));
	if(Property)
	{
		Property->SetValue_InContainer(params.EditorPlaySettings, &Instructions.MapPackageName);
	}
	
	//TODO: Maybe one day we want to integrate bAllowOnlineSubsystem...
	const FFunKSettings& settings = GetDefault<UFunKSettingsObject>()->Settings;

	const bool isStandalone = Instructions.IsStandaloneTest();
	if(isStandalone)
	{
		params.EditorPlaySettings->SetPlayNumberOfClients(1);
		params.EditorPlaySettings->bLaunchSeparateServer = false;
		params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
		SetFpsSettings(params.EditorPlaySettings, settings, false);
		
		GEditor->RequestPlaySession(params);
	}
	else
	{
		if(Instructions.IsListenServerTest())
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(3);
			params.EditorPlaySettings->bLaunchSeparateServer = false;
			params.EditorPlaySettings->SetRunUnderOneProcess(settings.RunTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_ListenServer);
			SetFpsSettings(params.EditorPlaySettings, settings, false);
			
			GEditor->RequestPlaySession(params);
		}
		else if(Instructions.IsDedicatedServerTest())
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(2);
			params.EditorPlaySettings->bLaunchSeparateServer = true;
			params.EditorPlaySettings->SetRunUnderOneProcess(settings.RunTestUnderOneProcess);
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Client);
			SetFpsSettings(params.EditorPlaySettings, settings, true);
			
			GEditor->RequestPlaySession(params);
		}
		else
		{
			return false;
		}
	}

	// Immediately launch the session 
	GEditor->StartQueuedPlaySessionRequest();
	if(!isStandalone && !settings.RunTestUnderOneProcess)
		ProcessStartCapture.GetProcessIds(StartedProcesses);
	
	return FailHandler.CanProceed() && (settings.RunTestUnderOneProcess || isStandalone || (ProcessStartCapture.IsValid() && StartedProcesses.Num() == 2));
}

void UFunKEditorEnvironmentHandler::SetFpsSettings(ULevelEditorPlaySettings* playSettings, const FFunKSettings& funkSettings, bool isDedicated)
{
	if(funkSettings.FixedTickFrameRates.Num() <= 0)
		return;

	const int32 server = funkSettings.FixedTickFrameRates[0];
	if(isDedicated)
		playSettings->ServerFixedFPS = server;
	else
		playSettings->ClientFixedFPS.Add(server);

	int32 client1 = server;
	int32 client2 = server;
	if(funkSettings.FixedTickFrameRates.Num() == 2)
	{
		client1 = client2 = funkSettings.FixedTickFrameRates[1];
	}
	else if(funkSettings.FixedTickFrameRates.Num() > 2)
	{
		client1 = funkSettings.FixedTickFrameRates[1];
		client2 = funkSettings.FixedTickFrameRates[2];
	}

	playSettings->ClientFixedFPS.Add(client1);
	playSettings->ClientFixedFPS.Add(client2);
}

int32 UFunKEditorEnvironmentHandler::GetTargetReplicationControllerCount(const FFunKTestInstructions& Instructions)
{
	return Instructions.IsListenServerTest() || Instructions.IsDedicatedServerTest()
		? 2
		: 0;
}

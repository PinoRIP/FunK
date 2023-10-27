// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKEditorEnvironmentHandler.h"
#include "FunKSettingsObject.h"
#include "FunKTestInstructions.h"
#include "Captures/FunKFailedPieStartCapture.h"
#include "Captures/FunKNewProcessCapture.h"
#include "FunK.h"
#include "FunKLogging.h"
#include "FunKWorldSubsystem.h"
#include "Editor/EditorPerformanceSettings.h"
#include "EventBus/FunKEventBusSubsystem.h"

EFunKEnvironmentWorldState UFunKEditorEnvironmentHandler::UpdateWorldState(const FFunKTestInstructions& Instructions)
{
	bool IsWrongEnvironment = false;
	const bool IsEnvironmentCurrentlyRunning = IsEnvironmentRunning(Instructions, IsWrongEnvironment);
	if (!IsEnvironmentCurrentlyRunning || IsWrongEnvironment)
	{
		if (!WaitsForWorld)
		{
			if (IsWrongEnvironment)
			{
				if (IsEnvironmentCurrentlyRunning)
					GEditor->EndPlayMap();
				
				GEditor->EndPlayOnLocalPc();
				StartedProcesses.Empty(StartedProcesses.Num());
			}
		
			if (!StartEnvironment(Instructions))
				return EFunKEnvironmentWorldState::CantStart;
		
			WaitsForWorld = true;
		}
	}
	else
	{
		if (WaitsForWorld)
		{
			if (!World)
				World = GetCurrentPieWorldContext()->World();
			
            WaitsForWorld = !(World && World->HasBegunPlay());
			WaitsForControllers = !WaitsForWorld;
		}
		
		if (WaitsForControllers)
			WaitsForControllers = GetEventBus()->GetReadyReplicationControllerCount() != GetTargetReplicationControllerCount(Instructions);
	}
	
	return WaitsForWorld || WaitsForControllers
		? EFunKEnvironmentWorldState::IsStarting
		: EFunKEnvironmentWorldState::IsRunning;
}

UWorld* UFunKEditorEnvironmentHandler::GetWorld()
{
	return World;
}

bool UFunKEditorEnvironmentHandler::IsEnvironmentRunning(const FFunKTestInstructions& Instructions, bool& IsWrongEnvironmentRunning)
{
	IsWrongEnvironmentRunning = false;

	const FFunKSettings& Settings = GetDefault<UFunKSettingsObject>()->Settings;
	const FString CurrentPieWorldPackageName = GetCurrentPieWorldPackageName();
	const bool IsNoLocalMapRunning = (CurrentPieWorldPackageName.IsEmpty() || CurrentPieWorldPackageName.Len() <= 0);
	const bool LocalProcessOnly = Settings.RunTestUnderOneProcess || Instructions.IsStandaloneTest();
	if (IsNoLocalMapRunning)
	{
		IsWrongEnvironmentRunning = !LocalProcessOnly && IsHoldingSubprocesses();
		return false;
	}

	if (IsWrongEnvironmentType(Instructions))
	{
		IsWrongEnvironmentRunning = true;
		return true;
	}

	IsWrongEnvironmentRunning = CurrentPieWorldPackageName != Instructions.MapPackageName;
	if (IsWrongEnvironmentRunning || LocalProcessOnly)
		return true;

	for (const uint32 StartedProcess : StartedProcesses)
	{
		if (!FPlatformProcess::IsApplicationRunning(StartedProcess))
		{
			IsWrongEnvironmentRunning = true;
			break;
		}
	}

	return true;
}

bool UFunKEditorEnvironmentHandler::IsWrongEnvironmentType(const FFunKTestInstructions& Instructions) const
{
	const UWorld* PotentialWorld = GetCurrentPieWorldContext()->World();
	const ENetMode NetMode = GetCurrentPieWorldContext()->World()->GetNetMode();
	
	if (NetMode == NM_Standalone && Instructions.IsStandaloneTest())
		return false;
	
	if (NetMode == NM_ListenServer && Instructions.IsListenServerTest())
		return false;
	
	if (NetMode == NM_DedicatedServer && Instructions.IsDedicatedServerTest())
		return false;
	
	if (NetMode != NM_Client)
		return true;
	
	if (Instructions.IsStandaloneTest())
		return true;
	
	const UFunKWorldSubsystem* Subsystem = PotentialWorld->GetSubsystem<UFunKWorldSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(FunKLog, Error, TEXT("Missing FunK world subsystem"))
		return true;
	}

	if (Subsystem->IsServerDedicated() && Instructions.IsDedicatedServerTest())
		return false;
	
	return Instructions.IsListenServerTest();
}

FString UFunKEditorEnvironmentHandler::GetCurrentPieWorldPackageName()
{
	const FWorldContext* WorldContext = GetCurrentPieWorldContext();
	if (!WorldContext)
		return "";

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
	const FFunKFailedPieStartCapture FailHandler;
	const FFunKNewProcessCapture ProcessStartCapture;

	UEditorPerformanceSettings* EditorPerformanceSettings = GetMutableDefault<UEditorPerformanceSettings>();
	EditorPerformanceSettings->bThrottleCPUWhenNotForeground = false;
	EditorPerformanceSettings->bMonitorEditorPerformance = false;
	EditorPerformanceSettings->PostEditChange();
		
	FRequestPlaySessionParams Params;
	Params.EditorPlaySettings = NewObject<ULevelEditorPlaySettings>();
	Params.EditorPlaySettings->NewWindowHeight = 1080;
	Params.EditorPlaySettings->NewWindowWidth = 1920;
	
	Params.GlobalMapOverride = Instructions.MapPackageName;
	Params.AdditionalStandaloneCommandLineParameters = FFunKModule::FunkTestStartParameter;

	if (const FProperty* Property = ULevelEditorPlaySettings::StaticClass()->FindPropertyByName(FName("ServerMapNameOverride")))
	{
		Property->SetValue_InContainer(Params.EditorPlaySettings, &Instructions.MapPackageName);
	}
	
	//TODO: Maybe one day we want to integrate bAllowOnlineSubsystem...
	const FFunKSettings& Settings = GetDefault<UFunKSettingsObject>()->Settings;

	const bool IsStandalone = Instructions.IsStandaloneTest();
	if (IsStandalone)
	{
		Params.EditorPlaySettings->SetPlayNumberOfClients(1);
		Params.EditorPlaySettings->bLaunchSeparateServer = false;
		Params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
		SetFpsSettings(Params.EditorPlaySettings, Settings, false);
		
		GEditor->RequestPlaySession(Params);
	}
	else
	{
		if (Instructions.IsListenServerTest())
		{
			Params.EditorPlaySettings->SetPlayNumberOfClients(3);
			Params.EditorPlaySettings->bLaunchSeparateServer = false;
			Params.EditorPlaySettings->SetRunUnderOneProcess(Settings.RunTestUnderOneProcess);
			Params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_ListenServer);
			SetFpsSettings(Params.EditorPlaySettings, Settings, false);
			
			GEditor->RequestPlaySession(Params);
		}
		else if (Instructions.IsDedicatedServerTest())
		{
			Params.EditorPlaySettings->SetPlayNumberOfClients(2);
			Params.EditorPlaySettings->bLaunchSeparateServer = true;
			Params.EditorPlaySettings->SetRunUnderOneProcess(Settings.RunTestUnderOneProcess);
			Params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Client);
			SetFpsSettings(Params.EditorPlaySettings, Settings, true);
			
			GEditor->RequestPlaySession(Params);
		}
		else
		{
			return false;
		}
	}

	// Immediately launch the session 
	GEditor->StartQueuedPlaySessionRequest();
	if (!IsStandalone && !Settings.RunTestUnderOneProcess)
		ProcessStartCapture.GetProcessIds(StartedProcesses);
	
	return FailHandler.CanProceed() && (Settings.RunTestUnderOneProcess || IsStandalone || (ProcessStartCapture.IsValid() && StartedProcesses.Num() == 2));
}

void UFunKEditorEnvironmentHandler::SetFpsSettings(ULevelEditorPlaySettings* PlaySettings, const FFunKSettings& FunkSettings, const bool IsDedicated)
{
	if (FunkSettings.FixedTickFrameRates.Num() <= 0)
		return;

	const int32 ServerTickRate = FunkSettings.FixedTickFrameRates[0];
	if (IsDedicated)
		PlaySettings->ServerFixedFPS = ServerTickRate;
	else
		PlaySettings->ClientFixedFPS.Add(ServerTickRate);

	int32 Client1TickRate = ServerTickRate;
	int32 Client2TickRate = ServerTickRate;
	if (FunkSettings.FixedTickFrameRates.Num() == 2)
	{
		Client1TickRate = Client2TickRate = FunkSettings.FixedTickFrameRates[1];
	}
	else if (FunkSettings.FixedTickFrameRates.Num() > 2)
	{
		Client1TickRate = FunkSettings.FixedTickFrameRates[1];
		Client2TickRate = FunkSettings.FixedTickFrameRates[2];
	}

	PlaySettings->ClientFixedFPS.Add(Client1TickRate);
	PlaySettings->ClientFixedFPS.Add(Client2TickRate);
}

int32 UFunKEditorEnvironmentHandler::GetTargetReplicationControllerCount(const FFunKTestInstructions& Instructions)
{
	return Instructions.IsListenServerTest() || Instructions.IsDedicatedServerTest()
		? 2
		: 0;
}

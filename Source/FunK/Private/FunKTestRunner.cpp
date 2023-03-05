// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKTestRunner.h"

#include "FileHelpers.h"
#include "FunK.h"
#include "FunKAutomationEntry.h"
#include "FunKEngineSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "Sinks/FunKExtAutomationSink.h"
#include "Sinks/FunKInProcAutomationSink.h"
#include "Sinks/FunKLogSink.h"
#include "Sinks/FunKSink.h"

void UFunKTestRunner::Init(UFunKEngineSubsystem* funKEngineSubsystem, EFunKTestRunnerType RunType)
{
	Type = RunType;
	FunKEngineSubsystem = funKEngineSubsystem;

	TArray<TSubclassOf<UFunKSink>> sinkTypes;
	GetSinks(sinkTypes);

	Sinks.Reserve(sinkTypes.Num());
	for (const TSubclassOf<UFunKSink>& sinkType : sinkTypes)
	{
		Sinks.Add(NewSink(sinkType));
	}
}

bool UFunKTestRunner::Prepare(const FFunKTestInstructions& Instructions)
{
	if(Type == EFunKTestRunnerType::LocalInProc)
	{
		return StartEnvironment(Instructions);
	}
	else
	{
		return true;
	}
}

void UFunKTestRunner::Start()
{	
	IsStarted = true;
	RaiseStartEvent();
}

bool UFunKTestRunner::Next()
{
	if(!IsStarted)
	{
		RaiseErrorEvent("The Test run ist not active anymore!", "UFunKTestRun::Next");
		return true;
	}

	UWorld* TestWorld = nullptr;
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for ( const FWorldContext& Context : WorldContexts )
	{
		if ( ( ( Context.WorldType == EWorldType::PIE ) || ( Context.WorldType == EWorldType::Game ) ) && ( Context.World() != NULL ) )
		{
			TestWorld = Context.World();
			break;
		}
	}
	
//TODO: TEST EXEC

	RaiseInfoEvent("WAIT");
	return false;
}

void UFunKTestRunner::End()
{
	IsStarted = false;
	for (UFunKSink* FunKSink : Sinks)
	{
		if(FunKSink)
		{
			FunKSink->End(this);
		}
	}
	Sinks.Empty();
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
	for (TWeakObjectPtr<UFunKSink> FunKSink : Sinks)
	{
		if(FunKSink.IsValid())
		{
			FunKSink->RaiseEvent(raisedEvent, this);
		}
	}
}

UFunKEngineSubsystem* UFunKTestRunner::GetSubsystem() const
{
	return FunKEngineSubsystem.Get();
}

const FString& UFunKTestRunner::GetParameter()
{
	return ActiveTestInstructions.Params;
}

void UFunKTestRunner::RaiseStartEvent()
{
	RaiseInfoEvent("FunK Start");
}

void UFunKTestRunner::GetSinks(TArray<TSubclassOf<UFunKSink>>& outSinks)
{
	outSinks.Add(TSubclassOf<UFunKSink>(UFunKLogSink::StaticClass()));
	if(UFunKInProcAutomationSink::IsAvailable())
	{
		outSinks.Add(TSubclassOf<UFunKSink>(UFunKInProcAutomationSink::StaticClass()));
	}
	else
	{
		outSinks.Add(TSubclassOf<UFunKSink>(UFunKExtAutomationSink::StaticClass()));
	}
}

UFunKSink* UFunKTestRunner::NewSink(TSubclassOf<UFunKSink> sinkType)
{
	UFunKSink* sink = NewObject<UFunKSink>(this, sinkType);
	sink->Init(this);
	return sink;
}

bool UFunKTestRunner::StartEnvironment(const FFunKTestInstructions& Instructions)
{
	struct FFailedGameStartHandler
	{
		bool bCanProceed;

		FFailedGameStartHandler()
		{
			bCanProceed = true;
			FEditorDelegates::EndPIE.AddRaw(this, &FFailedGameStartHandler::OnEndPIE);
		}

		~FFailedGameStartHandler()
		{
			FEditorDelegates::EndPIE.RemoveAll(this);
		}

		bool CanProceed() const { return bCanProceed; }

		void OnEndPIE(const bool bInSimulateInEditor)
		{
			bCanProceed = false;
		}
	};

	bool bLoadAsTemplate = false;
	bool bShowProgress = false;
	bool bForceReload = Instructions.MapTestName != ActiveTestInstructions.MapTestName || Instructions.Params != ActiveTestInstructions.Params;

	bool bNeedLoadEditorMap = true;
	bool bNeedPieStart = true;
	bool bPieRunning = false;

	//check existing worlds
	const TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();
	for (auto& Context : WorldContexts)
	{
		if (Context.World())
		{
			FString WorldPackage = Context.World()->GetOutermost()->GetName();

			if (Context.WorldType == EWorldType::PIE)
			{
				//don't quit!  This was triggered while pie was already running!
				bNeedPieStart = Instructions.MapPackageName != UWorld::StripPIEPrefixFromPackageName(WorldPackage, Context.World()->StreamingLevelsPrefix);
				bPieRunning = true;
				break;
			}
			else if (Context.WorldType == EWorldType::Editor)
			{
				bNeedLoadEditorMap = Instructions.MapPackageName != WorldPackage;
			}
		}
	}

	if (bNeedLoadEditorMap || bForceReload)
	{
		if (bPieRunning)
		{
			GEditor->EndPlayMap();
		}
		FEditorFileUtils::LoadMap(*Instructions.MapPackageName, bLoadAsTemplate, bShowProgress);
		bNeedPieStart = true;
	}
	
	if(bNeedPieStart)
	{
		FFailedGameStartHandler FailHandler;
		
		FRequestPlaySessionParams params;
		params.EditorPlaySettings = NewObject<ULevelEditorPlaySettings>();
		params.EditorPlaySettings->NewWindowHeight = 315;
		params.EditorPlaySettings->NewWindowWidth = 560;
		
		//TODO: Maybe one day we want to integrate bAllowOnlineSubsystem...

		if(Instructions.Params.Contains(FFunKModule::FunkStandaloneParameter))
		{
			params.EditorPlaySettings->SetPlayNumberOfClients(1);
			params.EditorPlaySettings->bLaunchSeparateServer = false;
			params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
			
			GEditor->RequestPlaySession(params);
		}
		else
		{
			if(Instructions.Params.Contains(FFunKModule::FunkListenParameter))
			{
				params.EditorPlaySettings->SetPlayNumberOfClients(3);
				params.EditorPlaySettings->bLaunchSeparateServer = false;
				params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_ListenServer);
				GEditor->RequestPlaySession(params);
			}
			else if(Instructions.Params.Contains(FFunKModule::FunkDedicatedParameter))
			{
				params.EditorPlaySettings->SetPlayNumberOfClients(2);
				params.EditorPlaySettings->bLaunchSeparateServer = true;
				params.EditorPlaySettings->SetPlayNetMode(EPlayNetMode::PIE_Client);
				GEditor->RequestPlaySession(params);
			}
			else
			{
				RaiseErrorEvent("Invalid Parameter at start");
				return false;
			}
		}

		// Immediately launch the session 
		GEditor->StartQueuedPlaySessionRequest();
		return FailHandler.CanProceed();
	}

	return true;
}

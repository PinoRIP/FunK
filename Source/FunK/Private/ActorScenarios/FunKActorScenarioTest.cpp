// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenarios/FunKActorScenarioTest.h"
#include "FunKWorldTestController.h"
#include "ActorScenarios/FunKActorScenarioComponent.h"
#include "Net/UnrealNetwork.h"
#include "Stages/FunKStagesSetup.h"


FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionStandalone = FString("Standalone");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionDedicatedServerClientToServer = FString("DedicatedServer Client->Server");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionDedicatedServerServerToClient = FString("DedicatedServer Server->Client");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionDedicatedServerClientToClient = FString("DedicatedServer Client->Client");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionListenServerClientToServer = FString("ListenServer Client->Server");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionListenServerServerToClient = FString("ListenServer Server->Client");
FString AFunKActorScenarioTest::FunKTestActorScenarioStageExtensionListenServerClientToClient = FString("ListenServer Client->Client");

// Sets default values
AFunKActorScenarioTest::AFunKActorScenarioTest()
	: AFunKTestBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFunKActorScenarioTest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( AFunKActorScenarioTest, AcquiredActors );
}

void AFunKActorScenarioTest::OnBeginStage(const FName& StageName)
{
	FString newStageScenario = ParseStageScenario(StageName.ToString());
	if(CurrentStageScenario != newStageScenario)
	{
		OnStageScenarioChanged();
		CurrentStageScenario = newStageScenario;
	}
	
	if(!IsCurrentStageScenarioFinished)
	{
		Super::OnBeginStage(StageName);
	}
	else
	{
		FinishStage(EFunKTestResult::Skipped, "Skipping stage!");
	}
}

void AFunKActorScenarioTest::OnFinish(const FString& Message)
{
	CurrentStageScenario.Empty();
	
	Super::OnFinish(Message);
}

void AFunKActorScenarioTest::SetupStages(FFunKStagesSetup& stages)
{
	Super::SetupStages(stages);

	TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
	GetActorScenarioComponents(ActorScenarioComponents);
	
	if(ActorScenarioComponents.Num() > 0)
	{
		SetupStages(stages, ActorScenarioComponents);
	}

	if(stages.Num() <= 0)
	{
		stages.AddStage(AFunKActorScenarioTest, ErrorFallbackStage);
	}
}

void AFunKActorScenarioTest::InvokeAssume()
{
	if(!Assume())
	{
		FinishStage(EFunKTestResult::Skipped, "Assumption not met");
	}
}

void AFunKActorScenarioTest::InvokeArrange()
{
	if(Arrange())
	{
		Super::FinishStage();
	}
}

void AFunKActorScenarioTest::InvokeAssert()
{
	if(Assert() && !IsFinished())
	{
		Super::FinishStage();
	}
}

bool AFunKActorScenarioTest::Assume()
{
	return BpAssume();
}

bool AFunKActorScenarioTest::BpAssume_Implementation()
{
	return true;
}

void AFunKActorScenarioTest::ArrangeScenario()
{
	const ENetMode NetMode = GetNetMode();
	
	TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
	GetActorScenarioComponents(ActorScenarioComponents);

	const FFunKStage* stage = GetCurrentStage();
	if(!stage)
	{
		FinishStage(EFunKTestResult::Error, "Missing stage config");
	}

	if(NetMode != NM_Client)
	{
		const FString StageName = stage->Name.ToString();
		for (UFunKActorScenarioComponent* ActorScenarioComponent : ActorScenarioComponents)
		{
			AActor* actor = ActorScenarioComponent->AcquireActor();

			if(NetMode != NM_Standalone)
			{		
				if(!actor->GetIsReplicated())
					actor->SetReplicates(true);
				
				if(ActorScenarioComponent->IsOppositionActor)
				{
					if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToServer))
						actor->SetOwner(GetCurrentController());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerServerToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[1]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToServer))
						actor->SetOwner(GetCurrentController());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerServerToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[1]->GetOwner());
				}
				else
				{
					if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToServer))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerServerToClient))
						actor->SetOwner(GetCurrentController());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToServer))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerServerToClient))
						actor->SetOwner(GetCurrentController());
					else if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToClient))
						actor->SetOwner(GetCurrentController()->GetSpawnedController()[0]->GetOwner());
				}
			}
			
			AcquiredActors.Add(actor);
		}
	}

	CheckArrangeScenarioFinish(0.f);
}

void AFunKActorScenarioTest::CheckArrangeScenarioFinish(float DeltaTime)
{
	TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
	GetActorScenarioComponents(ActorScenarioComponents);

	if(AcquiredActors.Num() == ActorScenarioComponents.Num())
	{
		Super::FinishStage();
	}
}

bool AFunKActorScenarioTest::Arrange()
{
	return BpArrange();
}

bool AFunKActorScenarioTest::BpArrange_Implementation()
{
	return true;
}

void AFunKActorScenarioTest::Act()
{
	BpAct();
}

void AFunKActorScenarioTest::BpAct_Implementation()
{
}

bool AFunKActorScenarioTest::Assert()
{
	return BpAssert();
}

bool AFunKActorScenarioTest::HasMoreScenarios() const
{
	const int32 index = GetCurrentStageIndex() + 1;
	if(!IsValidStageIndex(index))
		return false;

	const TArray<FFunKStage>& stages = GetStages()->Stages;
	for (int32 i = index; i < stages.Num(); ++i)
	{
		if (!stages[index].Name.ToString().Contains(CurrentStageScenario))
			return true;
	}

	return false;
}

void AFunKActorScenarioTest::OnStageScenarioChanged()
{
	IsCurrentStageScenarioFinished = false;

	if(GetNetMode() != NM_Client && AcquiredActors.Num() > 0)
	{
		TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
		GetActorScenarioComponents(ActorScenarioComponents);
		for (int32 i = 0; i < ActorScenarioComponents.Num(); ++i)
		{
			AcquiredActors[i]->SetOwner(nullptr);
			ActorScenarioComponents[i]->ReleaseActor(AcquiredActors[i]);
		}
	}

	AcquiredActors.Empty(AcquiredActors.Num());
}

void AFunKActorScenarioTest::FinishStage(EFunKTestResult TestResult, const FString& Message)
{
	if(!IsCurrentStageScenarioFinished)
		IsCurrentStageScenarioFinished = TestResult != EFunKTestResult::None;

	if(IsCurrentStageScenarioFinished && HasMoreScenarios())
	{
		RaiseEvent(CreateEvent(TestResult, "Actor scenario stages finished: " + Message).AddToContext(GetStageName().ToString()));
		TestResult = EFunKTestResult::None;
	}
	
	Super::FinishStage(TestResult, Message);
}

AActor* AFunKActorScenarioTest::GetAcquireActorByComponent(UFunKActorScenarioComponent* Component)
{
	TArray<UFunKActorScenarioComponent*> ActorScenarioComponent;
	GetActorScenarioComponents(ActorScenarioComponent);

	for (int i = 0; i < ActorScenarioComponent.Num(); ++i)
	{
		if(ActorScenarioComponent[i] == Component)
			return AcquiredActors[i];
	}

	return nullptr;
}

bool AFunKActorScenarioTest::BpAssert_Implementation()
{
	return true;
}

void AFunKActorScenarioTest::ErrorFallbackStage()
{
	FinishStage(EFunKTestResult::Error, "Actor scenario needs actor scenario components!");
}

void AFunKActorScenarioTest::SetupStages(FFunKStagesSetup& stages, TArray<UFunKActorScenarioComponent*>& actorScenarioComponents)
{
	int32 standaloneCount = 0, dedicatedServerCount = 0, dedicatedServerClientCount = 0, listenServerCount = 0, listenServerClientCount = 0, dedicatedServerOppositionCount = 0, dedicatedServerClientOppositionCount = 0, listenServerOppositionCount = 0, listenServerClientOppositionCount = 0;;
	for (const UFunKActorScenarioComponent* ActorScenarioComponent : actorScenarioComponents)
	{
		standaloneCount = standaloneCount + (ActorScenarioComponent->IsStandaloneRelevant ? 1 : 0);

		if(ActorScenarioComponent->IsOppositionActor)
		{
			dedicatedServerOppositionCount = dedicatedServerOppositionCount + (ActorScenarioComponent->IsDedicatedServerRelevant ? 1 : 0);
			dedicatedServerClientOppositionCount = dedicatedServerClientOppositionCount + (ActorScenarioComponent->IsDedicatedServerClientRelevant ? 1 : 0);
			listenServerOppositionCount = listenServerOppositionCount + (ActorScenarioComponent->IsListenServerRelevant ? 1 : 0);
			listenServerClientOppositionCount = listenServerClientOppositionCount + (ActorScenarioComponent->IsListenServerClientRelevant ? 1 : 0);
		}
		else
		{
			dedicatedServerCount = dedicatedServerCount + (ActorScenarioComponent->IsDedicatedServerRelevant ? 1 : 0);
			dedicatedServerClientCount = dedicatedServerClientCount + (ActorScenarioComponent->IsDedicatedServerClientRelevant ? 1 : 0);
			listenServerCount = listenServerCount + (ActorScenarioComponent->IsListenServerRelevant ? 1 : 0);
			listenServerClientCount = listenServerClientCount + (ActorScenarioComponent->IsListenServerClientRelevant ? 1 : 0);
		}
	}
	
	AddScenarioStages(stages, FunKTestActorScenarioStageExtensionStandalone, standaloneCount > 0, false, false, false, false);
	
	AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToServer, false, dedicatedServerCount > 0, dedicatedServerClientCount > 0, false, false);
	if(dedicatedServerOppositionCount > 0)
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerServerToClient, false, dedicatedServerCount > 0, dedicatedServerClientCount > 0, false, false);
	if(dedicatedServerClientOppositionCount > 0)
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToClient, false, dedicatedServerCount > 0, dedicatedServerClientCount > 0, false, false);
	
	AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToServer, false, false, false, listenServerCount > 0, listenServerClientCount > 0);
	if(listenServerOppositionCount > 0)
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerServerToClient, false, false, false, listenServerCount > 0, listenServerClientCount > 0);
	if(listenServerClientOppositionCount > 0)
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToClient, false, false, false, listenServerCount > 0, listenServerClientCount > 0);
}

void AFunKActorScenarioTest::AddScenarioStages(FFunKStagesSetup& stages, const FString& name, bool standalone, bool dedicated, bool dedicatedClient, bool listen, bool listenClient)
{
	if(standalone || dedicated || dedicatedClient || listen || listenClient)
	{
		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("ArrangeScenario for " + name), &AFunKActorScenarioTest::ArrangeScenario)
			.WithTickDelegate<AFunKActorScenarioTest>(&AFunKActorScenarioTest::CheckArrangeScenarioFinish)
			.UpdateTimeLimit(ArrangeScenarioTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated)
			.SetRunOnDedicatedServerClient(dedicatedClient)
			.SetRunOnListenServer(listen)
			.SetRunOnListenServerClient(listenClient);

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Arrange for " + name), &AFunKActorScenarioTest::InvokeArrange)
			.UpdateTimeLimit(ArrangeTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated)
			.SetRunOnDedicatedServerClient(dedicatedClient)
			.SetRunOnListenServer(listen)
			.SetRunOnListenServerClient(listenClient);

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Act for " + name), &AFunKActorScenarioTest::Act)
			.UpdateTimeLimit(ActTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated)
			.SetRunOnDedicatedServerClient(dedicatedClient)
			.SetRunOnListenServer(listen)
			.SetRunOnListenServerClient(listenClient);

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Assert for " + name), &AFunKActorScenarioTest::InvokeAssert)
			.UpdateTimeLimit(AssertTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated || (dedicatedClient && IsAlsoAssertingOnDedicatedServer))
			.SetRunOnDedicatedServerClient(dedicatedClient || (dedicated && IsAlsoAssertingOnDedicatedServerClient))
			.SetRunOnListenServer(listen || (listenClient && IsAlsoAssertingOnListenServer))
			.SetRunOnListenServerClient(listenClient || (listen && IsAlsoAssertingOnListenServerClient));
	}
}

void AFunKActorScenarioTest::GetActorScenarioComponents(TArray<UFunKActorScenarioComponent*>& ActorScenarioComponents)
{
	GetComponents(ActorScenarioComponents);
}

FString AFunKActorScenarioTest::ParseStageScenario(const FString& StageName)
{
	if(StageName.Contains(FunKTestActorScenarioStageExtensionStandalone))
		return FunKTestActorScenarioStageExtensionStandalone;

	if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToServer))
		return FunKTestActorScenarioStageExtensionDedicatedServerClientToServer;

	if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerServerToClient))
		return FunKTestActorScenarioStageExtensionDedicatedServerServerToClient;

	if(StageName.Contains(FunKTestActorScenarioStageExtensionDedicatedServerClientToClient))
		return FunKTestActorScenarioStageExtensionDedicatedServerClientToClient;

	if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToServer))
		return FunKTestActorScenarioStageExtensionListenServerClientToServer;
	
	if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerServerToClient))
		return FunKTestActorScenarioStageExtensionListenServerServerToClient;

	if(StageName.Contains(FunKTestActorScenarioStageExtensionListenServerClientToClient))
		return FunKTestActorScenarioStageExtensionListenServerClientToClient;

	check(false);
	return FString();
}


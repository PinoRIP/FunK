// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenarios/FunKActorScenarioTest.h"
#include "FunKWorldTestController.h"
#include "ActorScenarios/FunKActorScenarioComponent.h"
#include "Engine/ActorChannel.h"
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
	bReplicates = true;

	AlsoAssertOn = static_cast<int32>(EFunKTestLocationTarget::DedicatedServer | EFunKTestLocationTarget::ListenServer | EFunKTestLocationTarget::DedicatedServerClient | EFunKTestLocationTarget::ListenServerClient);

	ArrangeScenarioTimeLimit.Message = FText::FromString("Arrange scenario time limit reached!");
	ArrangeTimeLimit.Message = FText::FromString("Arrange time limit reached!");
	ActTimeLimit.Message = FText::FromString("Act time limit reached!");
	AssertTimeLimit.Message = FText::FromString("Assert time limit reached!");
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
		OnStageScenarioChanged(StageName, newStageScenario);
	}
	
	if(!IsCurrentStageScenarioFinished)
	{
		if(IsSkippingClient2())
		{
			if(IsLastStage())
			{
				// Client 2 is being executed because the TestBase doesn't differentiate between Client_1 & Client_2 (working as designed). If the test it self doesn't need this anymore this is basically a success...
				FinishStage(EFunKTestResult::Succeeded, "");
			}
			else
			{
				Super::FinishStage();
			}
			return;
		}
		
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

void AFunKActorScenarioTest::Tick(float DeltaTime)
{
	if(IsCurrentStageScenarioFinished)
		return;
	
	Super::Tick(DeltaTime);
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
				check(actor->GetIsReplicated());
				
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

				actor->ForceNetUpdate();

				actor->bAlwaysRelevant = true;
				actor->SetAutonomousProxy(actor->GetOwner() != GetCurrentController());
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
		for (int i = 0; i < AcquiredActors.Num(); ++i)
		{
			const AActor* AcquiredActor = AcquiredActors[i];
			if(!AcquiredActor || !AcquiredActor->HasActorBegunPlay())
				return;
		}

		if(IsLastStage())
		{
			// ArrangeScenario ist just for setup. If the test it self doesn't need this anymore this is basically a success...
			FinishStage(EFunKTestResult::Succeeded, "");
		}
		else
		{
			Super::FinishStage();
		}		
	}
}

void AFunKActorScenarioTest::Arrange()
{
	if(IsBpEventImplemented(GET_FUNCTION_NAME_CHECKED(AFunKActorScenarioTest, BpArrange)))
	{
		BpArrange();
	}
	else
	{
		Super::FinishStage();
	}
}

void AFunKActorScenarioTest::BpArrange_Implementation()
{
}

void AFunKActorScenarioTest::BpArrangeTick_Implementation(float DeltaTime)
{
}

void AFunKActorScenarioTest::Act()
{
	BpAct();
}

void AFunKActorScenarioTest::BpAct_Implementation()
{
}

void AFunKActorScenarioTest::BpActTick_Implementation(float DeltaTime)
{
}

void AFunKActorScenarioTest::Assert()
{
	if(IsBpEventImplemented(GET_FUNCTION_NAME_CHECKED(AFunKActorScenarioTest, BpAssert)))
	{
		BpAssert();
	}
	else if(!IsFinished())
	{
		Super::FinishStage();
	}
}

void AFunKActorScenarioTest::BpAssert_Implementation()
{
}

void AFunKActorScenarioTest::BpAssertTick_Implementation(float DeltaTime)
{
}

bool AFunKActorScenarioTest::HasMoreScenarios() const
{
	const int32 index = GetCurrentStageIndex() + 1;
	if(!IsValidStageIndex(index))
		return false;

	const TArray<FFunKStage>& stages = GetStages()->Stages;
	for (int32 i = index; i < stages.Num(); ++i)
	{
		if (!stages[i].Name.ToString().Contains(CurrentStageScenario) && IsExecutingStage(stages[i]))
			return true;
	}

	return false;
}

void AFunKActorScenarioTest::OnStageScenarioChanged(const FName& StageName, const FString& NewStageScenario)
{
	IsCurrentStageScenarioFinished = false;

	if(!CurrentStageScenario.IsEmpty())
	{
		const ENetMode netMode = GetNetMode();
		if(netMode != NM_Client && AcquiredActors.Num() > 0)
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
	
	CurrentStageScenario = NewStageScenario;
}

bool AFunKActorScenarioTest::IsSkippingClient2() const
{
	const ENetMode netMode = GetNetMode();
	if(netMode != NM_Client || CurrentStageScenario == FunKTestActorScenarioStageExtensionDedicatedServerClientToClient || CurrentStageScenario == FunKTestActorScenarioStageExtensionListenServerClientToClient)
		return false;
	
	const FString StageName = GetStageName().ToString();

	if(StageName.Contains("ArrangeScenario"))
		return false;

	const AFunKWorldTestController* currentController = GetCurrentController();
	const bool isDedicated = currentController->GetIsServerDedicated();
	const bool isClient2 = currentController->GetControllerNumber() == 2;

	if (StageName.Contains("Arrange") && isDedicated ? IsArrangeAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsArrangeAlsoOn(EFunKTestLocationTarget::ListenServerClient))
		return isClient2;

	if (StageName.Contains("Act") && isDedicated ? IsActAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsActAlsoOn(EFunKTestLocationTarget::ListenServerClient))
		return isClient2;

	if (StageName.Contains("Assert") && isDedicated ? IsAssertAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsAssertAlsoOn(EFunKTestLocationTarget::ListenServerClient))
		return isClient2;

	return true;
}

bool AFunKActorScenarioTest::IsArrangeAlsoOn(EFunKTestLocationTarget alsoOnTarget) const
{
	
	return IsAlsoOn(AlsoArrangeOn, alsoOnTarget);
}

bool AFunKActorScenarioTest::IsActAlsoOn(EFunKTestLocationTarget alsoOnTarget) const
{
	return IsAlsoOn(AlsoActOn, alsoOnTarget);
}

bool AFunKActorScenarioTest::IsAssertAlsoOn(EFunKTestLocationTarget alsoOnTarget) const
{
	return IsAlsoOn(AlsoAssertOn, alsoOnTarget);
}

bool AFunKActorScenarioTest::IsAlsoOn(int32 state, EFunKTestLocationTarget alsoOnTarget) const
{
	const int32 alsoOnTargetAs32 = static_cast<int32>(alsoOnTarget);
	return (alsoOnTargetAs32 & state) == alsoOnTargetAs32;
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

	if(AcquiredActors.Num() == ActorScenarioComponent.Num()) for (int i = 0; i < ActorScenarioComponent.Num(); ++i)
	{
		if(ActorScenarioComponent[i] == Component)
			return AcquiredActors[i];
	}

	return nullptr;
}

void AFunKActorScenarioTest::ErrorFallbackStage()
{
	FinishStage(EFunKTestResult::Error, "Actor scenario needs actor scenario components!");
}

void AFunKActorScenarioTest::SetupStages(FFunKStagesSetup& stages, TArray<UFunKActorScenarioComponent*>& actorScenarioComponents)
{
	int32 standaloneCount = 0, dedicatedServerCount = 0, dedicatedServerClientCount = 0, listenServerCount = 0, listenServerClientCount = 0, dedicatedServerOpposition = 0, dedicatedServerClientOppositionCount = 0,  listenServerOpposition = 0, listenServerClientOppositionCount = 0;;
	for (const UFunKActorScenarioComponent* ActorScenarioComponent : actorScenarioComponents)
	{
		standaloneCount = standaloneCount + (ActorScenarioComponent->IsStandaloneRelevant ? 1 : 0);

		if(ActorScenarioComponent->IsOppositionActor)
		{
			dedicatedServerOpposition = dedicatedServerOpposition + (ActorScenarioComponent->IsDedicatedServerClientRelevant ? 1 : 0);
			dedicatedServerClientOppositionCount = dedicatedServerClientOppositionCount + (ActorScenarioComponent->IsDedicatedServerClientRelevant ? 1 : 0);
			listenServerOpposition = listenServerOpposition + (ActorScenarioComponent->IsDedicatedServerClientRelevant ? 1 : 0);
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
	
	if(dedicatedServerCount > 0)
	{
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerServerToClient, false, dedicatedServerCount > 0, dedicatedServerClientOppositionCount > 0, false, false);
	}
	
	if (dedicatedServerClientCount > 0)
	{
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToServer, false, dedicatedServerOpposition > 0, true, false, false);
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToClient, false, dedicatedServerClientOppositionCount > 0, dedicatedServerClientOppositionCount > 0, false, false);
	}

	if(listenServerCount > 0)
	{
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerServerToClient, false, false, false, listenServerCount > 0, listenServerClientOppositionCount > 0);
	}
	
	if (listenServerClientCount > 0)
	{
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToServer, false, false, false, listenServerOpposition > 0, true);
		AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToClient, false, false, false, listenServerClientOppositionCount > 0, listenServerClientOppositionCount > 0);
	}
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
			.SetRunOnDedicatedServer(dedicated || (!dedicated && dedicatedClient))
			.SetRunOnDedicatedServerClient(dedicatedClient || (!dedicatedClient && dedicated))
			.SetRunOnListenServer(listen || (!listen && listenClient))
			.SetRunOnListenServerClient(listenClient || (!listenClient && listen));

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Arrange for " + name), &AFunKActorScenarioTest::Arrange)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpArrangeTick)
			.UpdateTimeLimit(ArrangeTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated || (dedicatedClient && IsArrangeAlsoOn(EFunKTestLocationTarget::DedicatedServer)))
			.SetRunOnDedicatedServerClient(dedicatedClient || (dedicated && IsArrangeAlsoOn(EFunKTestLocationTarget::DedicatedServerClient)))
			.SetRunOnListenServer(listen || (listenClient && IsArrangeAlsoOn(EFunKTestLocationTarget::ListenServer)))
			.SetRunOnListenServerClient(listenClient || (listen && IsArrangeAlsoOn(EFunKTestLocationTarget::ListenServerClient)));

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Act for " + name), &AFunKActorScenarioTest::Act)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpActTick)
			.UpdateTimeLimit(ActTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated || (dedicatedClient && IsActAlsoOn(EFunKTestLocationTarget::DedicatedServer)))
			.SetRunOnDedicatedServerClient(dedicatedClient || (dedicated && IsActAlsoOn(EFunKTestLocationTarget::DedicatedServerClient)))
			.SetRunOnListenServer(listen || (listenClient && IsActAlsoOn(EFunKTestLocationTarget::ListenServer)))
			.SetRunOnListenServerClient(listenClient || (listen && IsActAlsoOn(EFunKTestLocationTarget::ListenServerClient)));

		stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Assert for " + name), &AFunKActorScenarioTest::Assert)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpAssertTick)
			.UpdateTimeLimit(AssertTimeLimit)
			.SetRunOnStandalone(standalone)
			.SetRunOnDedicatedServer(dedicated || (dedicatedClient && IsAssertAlsoOn(EFunKTestLocationTarget::DedicatedServer)))
			.SetRunOnDedicatedServerClient(dedicatedClient || (dedicated && IsAssertAlsoOn(EFunKTestLocationTarget::DedicatedServerClient)))
			.SetRunOnListenServer(listen || (listenClient && IsAssertAlsoOn(EFunKTestLocationTarget::ListenServer)))
			.SetRunOnListenServerClient(listenClient || (listen && IsAssertAlsoOn(EFunKTestLocationTarget::ListenServerClient)));
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


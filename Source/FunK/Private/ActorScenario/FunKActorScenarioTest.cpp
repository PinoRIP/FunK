// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorScenario/FunKActorScenarioTest.h"
#include "FunKWorldSubsystem.h"
#include "ActorScenario/FunKActorScenarioComponent.h"
#include "Internal/ActorScenario/FunKTestLocationTarget.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Internal/Setup/FunKStagesSetup.h"


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

void AFunKActorScenarioTest::OnBeginStage()
{
	const FName StageName = GetStageName();
	const FString StageScenario = ParseStageScenario(StageName.ToString());
	if(CurrentStageScenario != StageScenario)
	{
		OnStageScenarioChanged(StageName, StageScenario);
	}
	
	Super::OnBeginStage();
}

void AFunKActorScenarioTest::OnFinish(const FString& Message)
{
	CurrentStageScenario.Empty();
	
	Super::OnFinish(Message);
}

void AFunKActorScenarioTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AFunKActorScenarioTest::IsListenServerScenario() const
{
	return CurrentScenario.IsListenServer();
}

bool AFunKActorScenarioTest::IsDedicatedServerScenario() const
{
	return CurrentScenario.IsDedicatedServer();
}

bool AFunKActorScenarioTest::IsServerToClientScenario() const
{
	return CurrentScenario.IsServerToClient();
}

bool AFunKActorScenarioTest::IsClientToServerScenario() const
{
	return CurrentScenario.IsClientToServer();
}

bool AFunKActorScenarioTest::IsClientToClientScenario() const
{
	return CurrentScenario.IsClientToClient();
}

bool AFunKActorScenarioTest::IsStandaloneScenario() const
{
	return CurrentScenario.IsStandalone();
}

bool AFunKActorScenarioTest::IsLocalOwnershipNone() const
{
	return GetNetMode() != NM_Client;
}

bool AFunKActorScenarioTest::IsLocalOwnershipAppositionPlayer() const
{
	const ENetMode NetMode = GetNetMode();
	if(NetMode == NM_Standalone) return true;
	return (CurrentScenario.IsFromServer() && NetMode != NM_Client) || (CurrentScenario.IsFromClient() && GetPeerIndex() == 1);
}

bool AFunKActorScenarioTest::IsLocalOwnershipOppositionPlayer() const
{
	const ENetMode NetMode = GetNetMode();
	if(NetMode == NM_Standalone) return false;
	return (CurrentScenario.IsToServer() && NetMode != NM_Client) || (CurrentScenario.IsToClient() && GetPeerIndex() == 2);
}

bool AFunKActorScenarioTest::IsLocalOwnershipAI() const
{
	return GetNetMode() != NM_Client;
}

const FFunKActorScenarioSetup& AFunKActorScenarioTest::GetScenarioSetup()
{
	TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
	GetActorScenarioComponents(ActorScenarioComponents);

	if(ActorScenarioComponents.Num() <= 0)
	{
		auto b = GetClass()->GetDefaultObject<AFunKActorScenarioTest>();
		b->GetActorScenarioComponents(ActorScenarioComponents);
	}

	bool HasNone = false, HasApposition = false, HasOpposition = false, HasAI = false;
	for (const UFunKActorScenarioComponent* ActorScenarioComponent : ActorScenarioComponents)
	{
		HasNone = HasNone || ActorScenarioComponent->ActorOwnership == EFunKActorScenarioOwnership::None;
		HasApposition = HasApposition || ActorScenarioComponent->ActorOwnership == EFunKActorScenarioOwnership::AppositionPlayer;
		HasOpposition = HasOpposition || ActorScenarioComponent->ActorOwnership == EFunKActorScenarioOwnership::OppositionPlayer;
		HasAI = HasAI || ActorScenarioComponent->ActorOwnership == EFunKActorScenarioOwnership::AI;
	}

	ScenarioSetup.TestStandaloneAvailable = !HasOpposition;
	ScenarioSetup.TestDedicatedServer.ClientToClient.IsAvailable = !HasNone && !HasAI;
	ScenarioSetup.TestDedicatedServer.ClientToServer.IsAvailable = !HasNone && !HasAI && !HasOpposition;
	ScenarioSetup.TestDedicatedServer.ServerToClient.IsAvailable = !HasApposition;
	ScenarioSetup.TestListenServer.ClientToClient.IsAvailable = !HasNone && !HasAI;
	ScenarioSetup.TestListenServer.ClientToServer.IsAvailable = !HasNone && !HasAI;
	ScenarioSetup.TestListenServer.ServerToClient.IsAvailable = true;

	return ScenarioSetup;
}

void AFunKActorScenarioTest::SetupStages(FFunKStagesSetup& stages)
{
	Super::SetupStages(stages);

	SetupScenarioStages(stages);

	if(stages.Num() <= 0)
	{
		stages.AddStage(AFunKActorScenarioTest, ErrorFallbackStage);
	}
}

bool AFunKActorScenarioTest::IsExecutingStage(const FFunKStage& stage) const
{
	return Super::IsExecutingStage(stage) && !IsSkippingClient2(stage);
}

void AFunKActorScenarioTest::InvokeAssume()
{
	if(!Assume())
	{
		FinishStage(EFunKStageResult::Skipped, "Assumption not met");
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
		FinishStage(EFunKStageResult::Error, "Missing stage config");
	}

	if(NetMode != NM_Client)
	{
		const FString StageName = stage->Name.ToString();
		
		for (UFunKActorScenarioComponent* ActorScenarioComponent : ActorScenarioComponents)
		{
			AActor* actor = ActorScenarioComponent->AcquireActor(CurrentScenario);			
			AcquiredActors.Add(actor);
		}
	}

	CheckArrangeScenarioFinish(0.f);
}

void AFunKActorScenarioTest::CheckArrangeScenarioFinish(float DeltaTime)
{
	TArray<UFunKActorScenarioComponent*> ActorScenarioComponents;
	GetActorScenarioComponents(ActorScenarioComponents);

	if(AcquiredActors.Num() != ActorScenarioComponents.Num())
		return;
	
	for (int i = 0; i < AcquiredActors.Num(); ++i)
	{
		if(!ActorScenarioComponents[i]->VerifyActor(AcquiredActors[i], CurrentScenario))
			return;
	}

	FinishStage();
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
				ActorScenarioComponents[i]->ReleaseActor(AcquiredActors[i], CurrentScenario);
			}

			AcquiredActors.Empty(AcquiredActors.Num());
		}
	}
	
	CurrentStageScenario = NewStageScenario;

	if (NewStageScenario == FunKTestActorScenarioStageExtensionStandalone)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::Standalone, EFunKTestLocationTarget::Standalone);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionDedicatedServerServerToClient)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::DedicatedServer, EFunKTestLocationTarget::DedicatedServerClient);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionDedicatedServerClientToServer)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::DedicatedServerClient, EFunKTestLocationTarget::DedicatedServer);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionDedicatedServerClientToClient)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::DedicatedServerClient, EFunKTestLocationTarget::DedicatedServerClient);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionListenServerServerToClient)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::ListenServer, EFunKTestLocationTarget::ListenServerClient);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionListenServerClientToServer)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::ListenServerClient, EFunKTestLocationTarget::ListenServer);
	else if (NewStageScenario == FunKTestActorScenarioStageExtensionListenServerClientToClient)
		CurrentScenario = FFunKActorScenario(EFunKTestLocationTarget::ListenServerClient, EFunKTestLocationTarget::ListenServerClient);
	else
		CurrentScenario = FFunKActorScenario();
}

bool AFunKActorScenarioTest::IsSkippingClient2(const FFunKStage& stage) const
{
	const ENetMode netMode = GetNetMode();
	if(netMode != NM_Client)
		return false;

	const bool isClient2 = GetPeerIndex() == 2;
	if(!isClient2)
		return false;

	const FString StageName = stage.Name.ToString();
	const FString Scenario = ParseStageScenario(StageName);
	if(Scenario == FunKTestActorScenarioStageExtensionDedicatedServerClientToClient || Scenario == FunKTestActorScenarioStageExtensionListenServerClientToClient)
		return false;

	if(StageName.Contains("ArrangeScenario"))
		return false;
	
	const bool isDedicated = GetWorldSubsystem()->IsServerDedicated();

	//if (StageName.Contains("Arrange") && isDedicated ? IsArrangeAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsArrangeAlsoOn(EFunKTestLocationTarget::ListenServerClient))
	//	return true;
//
	//if (StageName.Contains("Act") && isDedicated ? IsActAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsActAlsoOn(EFunKTestLocationTarget::ListenServerClient))
	//	return true;
//
	//if (StageName.Contains("Assert") && isDedicated ? IsAssertAlsoOn(EFunKTestLocationTarget::DedicatedServerClient) : IsAssertAlsoOn(EFunKTestLocationTarget::ListenServerClient))
	//	return true;

	return true;
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
	FinishStage(EFunKStageResult::Error, "Actor scenario needs actor scenario components!");
}

void AFunKActorScenarioTest::SetupScenarioStages(FFunKStagesSetup& stages)
{
	const FFunKActorScenarioSetup& setup = GetScenarioSetup();
	
	if(setup.TestStandalone)
	{
		const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionStandalone);
		StandaloneScenario.ArrangeScenario->SetRunOnStandalone(true);
		StandaloneScenario.Arrange->SetRunOnStandalone(true);
		StandaloneScenario.Act->SetRunOnStandalone(true);
		StandaloneScenario.Assert->SetRunOnStandalone(true);
	}

	if(setup.TestDedicatedServer.IsActive)
	{
		if(setup.TestDedicatedServer.ServerToClient.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerServerToClient);
			StandaloneScenario.ArrangeScenario->SetRunOnDedicated(true);
			
			StandaloneScenario.Arrange->SetRunOnDedicatedServer(true).SetRunOnDedicatedServerClient(setup.TestDedicatedServer.ServerToClient.OnOpposition.Arrange || setup.TestDedicatedServer.ServerToClient.OnThird.Arrange);
			StandaloneScenario.Act->SetRunOnDedicatedServer(true).SetRunOnDedicatedServerClient(setup.TestDedicatedServer.ServerToClient.OnOpposition.Act || setup.TestDedicatedServer.ServerToClient.OnThird.Act);
			StandaloneScenario.Assert->SetRunOnDedicatedServer(true).SetRunOnDedicatedServerClient(setup.TestDedicatedServer.ServerToClient.OnOpposition.Assert || setup.TestDedicatedServer.ServerToClient.OnThird.Assert);
		}

		if(setup.TestDedicatedServer.ClientToServer.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToServer);
			StandaloneScenario.ArrangeScenario->SetRunOnDedicated(true);
			
			StandaloneScenario.Arrange->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnOpposition.Arrange);
			StandaloneScenario.Act->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnOpposition.Act);
			StandaloneScenario.Assert->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnOpposition.Assert);
		}

		if(setup.TestDedicatedServer.ClientToClient.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionDedicatedServerClientToClient);
			StandaloneScenario.ArrangeScenario->SetRunOnDedicated(true);
			
			StandaloneScenario.Arrange->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnThird.Arrange);
			StandaloneScenario.Act->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnThird.Act);
			StandaloneScenario.Assert->SetRunOnDedicatedServerClient(true).SetRunOnDedicatedServer(setup.TestDedicatedServer.ServerToClient.OnThird.Assert);
		}
	}

	if(setup.TestListenServer.IsActive)
	{
		if(setup.TestListenServer.ServerToClient.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerServerToClient);
			StandaloneScenario.ArrangeScenario->SetRunOnListen(true);
			
			StandaloneScenario.Arrange->SetRunOnListenServer(true).SetRunOnListenServerClient(setup.TestListenServer.ServerToClient.OnOpposition.Arrange || setup.TestListenServer.ServerToClient.OnThird.Arrange);
			StandaloneScenario.Act->SetRunOnListenServer(true).SetRunOnListenServerClient(setup.TestListenServer.ServerToClient.OnOpposition.Act || setup.TestListenServer.ServerToClient.OnThird.Act);
			StandaloneScenario.Assert->SetRunOnListenServer(true).SetRunOnListenServerClient(setup.TestListenServer.ServerToClient.OnOpposition.Assert || setup.TestListenServer.ServerToClient.OnThird.Assert);
		}

		if(setup.TestListenServer.ClientToServer.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToServer);
			StandaloneScenario.ArrangeScenario->SetRunOnListen(true);
			
			StandaloneScenario.Arrange->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnOpposition.Arrange);
			StandaloneScenario.Act->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnOpposition.Act);
			StandaloneScenario.Assert->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnOpposition.Assert);
		}

		if(setup.TestListenServer.ClientToClient.IsActive)
		{
			const FFunKScenarioStages StandaloneScenario = AddScenarioStages(stages, FunKTestActorScenarioStageExtensionListenServerClientToClient);
			StandaloneScenario.ArrangeScenario->SetRunOnListen(true);
			
			StandaloneScenario.Arrange->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnThird.Arrange);
			StandaloneScenario.Act->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnThird.Act);
			StandaloneScenario.Assert->SetRunOnListenServerClient(true).SetRunOnListenServer(setup.TestListenServer.ServerToClient.OnThird.Assert);
		}
	}
}

FFunKScenarioStages AFunKActorScenarioTest::AddScenarioStages(FFunKStagesSetup& stages, const FString& name)
{
	return FFunKScenarioStages(
		&stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("ArrangeScenario for " + name), &AFunKActorScenarioTest::ArrangeScenario)
			.WithTickDelegate<AFunKActorScenarioTest>(&AFunKActorScenarioTest::CheckArrangeScenarioFinish)
			.UpdateTimeLimit(ArrangeScenarioTimeLimit),
		&stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Arrange for " + name), &AFunKActorScenarioTest::Arrange)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpArrangeTick)
			.UpdateTimeLimit(ArrangeTimeLimit),
		&stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Act for " + name), &AFunKActorScenarioTest::Act)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpActTick)
			.UpdateTimeLimit(ActTimeLimit),
		&stages
			.AddNamedLatentStage<AFunKActorScenarioTest>(FName("Assert for " + name), &AFunKActorScenarioTest::Assert)
			.WithOptionalBpTickDelegate(AFunKActorScenarioTest, BpAssertTick)
			.UpdateTimeLimit(AssertTimeLimit)
	);
}

void AFunKActorScenarioTest::GetActorScenarioComponents(TArray<UFunKActorScenarioComponent*>& ActorScenarioComponents) const
{
	GetComponents(ActorScenarioComponents);
}

FString AFunKActorScenarioTest::ParseStageScenario(const FString& StageName) const
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldSubsystem.h"

#include "FunKEngineSubsystem.h"
#include "FunKSettingsObject.h"
#include "FunKWorldTestController.h"

void UFunKWorldSubsystem::CheckLocalTestController()
{
	GetLocalTestController();

	if(GEngine)
	{
		//TODO: Move the get somewhere else...
		UFunKEngineSubsystem* funk = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>();
		if(funk && funk->IsRunning())
		{
			funk->CallbackTestWorldBeganPlay(GetWorld());
		}
	}
}

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if(!LocalTestController && GetWorld()->GetNetMode() != NM_Client)
	{
		LocalTestController = NewTestController();
		LocalTestController->SetupLocalTestController();
	}
	
	return LocalTestController;
}

void UFunKWorldSubsystem::SetLocalTestController(AFunKWorldTestController* localTestController)
{
	if(!LocalTestController && GetWorld()->GetNetMode() == NM_Client)
	{
		LocalTestController = localTestController;
		LocalTestController->SetupLocalTestController();
	}
}

bool UFunKWorldSubsystem::HasLocalTestController() const
{
	return !!LocalTestController;
}

AFunKWorldTestController* UFunKWorldSubsystem::NewTestController() const
{
	UClass* ReplicatedManagerClass = GetDefault<UFunKSettingsObject>()->Settings.WorldTestControllerClassOverride.Get();
	return ReplicatedManagerClass
		? GetWorld()->SpawnActor<AFunKWorldTestController>(ReplicatedManagerClass)
		: GetWorld()->SpawnActor<AFunKWorldTestController>();
}

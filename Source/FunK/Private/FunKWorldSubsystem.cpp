// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKWorldSubsystem.h"

#include "FunKEngineSubsystem.h"
#include "FunKWorldTestController.h"

void UFunKWorldSubsystem::CheckLocalTestController()
{
	GetLocalTestController();

	//TODO: Move the get somewhere else...
	UFunKEngineSubsystem* funk = GEngine->GetEngineSubsystem<UFunKEngineSubsystem>();
	if(funk && funk->IsRunning())
	{
		funk->CallbackTestWorldBeganPlay(GetWorld());
	}
}

AFunKWorldTestController* UFunKWorldSubsystem::GetLocalTestController()
{
	if(!LocalTestController && GetWorld()->GetNetMode() != NM_Client)
	{
		LocalTestController = NewTestController();
	}
	
	return LocalTestController;
}

void UFunKWorldSubsystem::SetLocalTestController(AFunKWorldTestController* localTestController)
{
	if(!LocalTestController && GetWorld()->GetNetMode() == NM_Client)
	{
		LocalTestController = localTestController;
	}
}

AFunKWorldTestController* UFunKWorldSubsystem::NewTestController() const
{
	//TODO
	return GetWorld()->SpawnActor<AFunKWorldTestController>();
}

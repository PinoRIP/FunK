// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Extensions/FunKSceneActorLocationResetHandler.h"

void UFunKSceneActorLocationResetHandler::Capture(AActor* Actor)
{
	Super::Capture(Actor);

	Location = Actor->GetActorLocation();
}

void UFunKSceneActorLocationResetHandler::Reset(AActor* Actor)
{
	Super::Reset(Actor);
	
	Actor->SetActorLocation(Location);
}

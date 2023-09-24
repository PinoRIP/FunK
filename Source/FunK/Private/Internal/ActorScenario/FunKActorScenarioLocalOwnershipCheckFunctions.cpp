// Fill out your copyright notice in the Description page of Project Settings.


#include "Internal/ActorScenario/FunKActorScenarioLocalOwnershipCheckFunctions.h"
#include "ActorScenario/FunKActorScenarioTest.h"

bool UFunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipNone(AFunKActorScenarioTest* Test)
{
	return Test && Test->IsLocalOwnershipNone();
}

bool UFunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipAppositionPlayer(AFunKActorScenarioTest* Test)
{
	return Test && Test->IsLocalOwnershipAppositionPlayer();
}

bool UFunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipOppositionPlayer(AFunKActorScenarioTest* Test)
{
	return Test && Test->IsLocalOwnershipOppositionPlayer();
}

bool UFunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipAI(AFunKActorScenarioTest* Test)
{
	return Test && Test->IsLocalOwnershipAI();
}
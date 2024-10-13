// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Automation/FunKTestInstructions.h"
#include "Old/FunK.h"

bool FFunKTestInstructions::IsStandaloneTest() const
{
	return Params.Contains(FFunKModule::FunkStandaloneParameter);
}

bool FFunKTestInstructions::IsDedicatedServerTest() const
{
	return Params.Contains(FFunKModule::FunkDedicatedParameter);
}

bool FFunKTestInstructions::IsListenServerTest() const
{
	return Params.Contains(FFunKModule::FunkListenParameter);
}

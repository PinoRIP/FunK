// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"
#include "FunK.h"
#include "FunKWorldSubsystem.h"
#include "FunKWorldTestController.h"
#include "FunKWorldTestExecution.h"


bool AFunKFunctionalTest::InvokeAssume()
{
	return BpAssume();
}

void AFunKFunctionalTest::InvokeStartSetup()
{
	BpStartSetup();
}

bool AFunKFunctionalTest::InvokeIsReady()
{
	return BpReady();
}

void AFunKFunctionalTest::InvokeStartTest()
{
	BpStartTest();
}

void AFunKFunctionalTest::InvokeCleanup()
{
	BpCleanup();
}

bool AFunKFunctionalTest::Assume()
{
	return true;
}

bool AFunKFunctionalTest::BpAssume_Implementation()
{
	return Assume();
}

void AFunKFunctionalTest::StartSetup()
{
}

bool AFunKFunctionalTest::IsReady()
{
	return true;
}

void AFunKFunctionalTest::StartTest()
{
}

void AFunKFunctionalTest::BpStartTest_Implementation()
{
	StartTest();
}

bool AFunKFunctionalTest::BpReady_Implementation()
{
	return IsReady();
}

void AFunKFunctionalTest::BpStartSetup_Implementation()
{
	StartSetup();
}

void AFunKFunctionalTest::Cleanup()
{
}

void AFunKFunctionalTest::BpCleanup_Implementation()
{
	Cleanup();
}

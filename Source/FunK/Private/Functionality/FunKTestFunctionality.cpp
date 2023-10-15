// Fill out your copyright notice in the Description page of Project Settings.


#include "Functionality/FunKTestFunctionality.h"

#include "FunKLogging.h"
#include "FunKTestBase.h"
#include "Events/FunKEvent.h"

AFunKTestBase* UFunKTestFunctionality::GetTest()
{
	return Test;
}

void UFunKTestFunctionality::Error(const FString& Message) const
{
	if(Test)
	{
		Test->RaiseEvent(FFunKEvent::Error(Message).Ref());
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("%s"), *Message)
	}
}

void UFunKTestFunctionality::OnAdded()
{
}

void UFunKTestFunctionality::OnBeginStage()
{
}

void UFunKTestFunctionality::OnFinishStage()
{
}

void UFunKTestFunctionality::OnRemoved()
{
}

FString UFunKTestFunctionality::GetReadableIdent() const
{
	return "";
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Extensions\FunKTestFragment.h"

#include "FunKLogging.h"
#include "FunKTestBase.h"
#include "Events/FunKEvent.h"

AFunKTestBase* UFunKTestFragment::GetTest()
{
	return Test;
}

void UFunKTestFragment::Error(const FString& Message) const
{
	if (Test)
	{
		Test->RaiseEvent(FFunKEvent::Error(Message).Ref());
	}
	else
	{
		UE_LOG(FunKLog, Error, TEXT("%s"), *Message)
	}
}

void UFunKTestFragment::OnAdded()
{
}

void UFunKTestFragment::OnBeginStage()
{
}

void UFunKTestFragment::OnFinishStage()
{
}

void UFunKTestFragment::OnRemoved()
{
}

FString UFunKTestFragment::GetLogName() const
{
	return "";
}
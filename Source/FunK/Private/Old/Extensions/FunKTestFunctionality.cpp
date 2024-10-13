// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Extensions/FunKTestFragment.h"

#include "Old/FunKLogging.h"
#include "Old/FunKTestBase.h"
#include "Old/Events/FunKEvent.h"

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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FunKNewProcessCapture.generated.h"

/**
 * 
 */
USTRUCT()
struct FFunKNewProcessCapture
{
	GENERATED_BODY()

public:
	FFunKNewProcessCapture() {
		ProcessID = 0;
		IsProcessIdSet = false;
		FEditorDelegates::BeginStandaloneLocalPlay.AddRaw(this, &FFunKNewProcessCapture::Callback);
	}

	~FFunKNewProcessCapture()
	{
		FEditorDelegates::BeginStandaloneLocalPlay.RemoveAll(this);
	}

	uint32 GetProcessId() const { return ProcessID; }
	bool IsValid() const { return IsProcessIdSet; }
private:
	uint32 ProcessID;
	bool IsProcessIdSet;

	void Callback(uint32 newProcessId)
	{
		ProcessID = newProcessId;
	}
};

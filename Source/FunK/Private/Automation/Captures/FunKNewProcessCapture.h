// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct FFunKNewProcessCapture
{
public:
	FFunKNewProcessCapture() {
		IsAnyProcessIdSet = false;
		FEditorDelegates::BeginStandaloneLocalPlay.AddRaw(this, &FFunKNewProcessCapture::Callback);
	}

	~FFunKNewProcessCapture()
	{
		FEditorDelegates::BeginStandaloneLocalPlay.RemoveAll(this);
	}

	void GetProcessIds(TArray<uint32>& processIDs) const { processIDs = ProcessIDs; }
	bool IsValid() const { return IsAnyProcessIdSet; }
	
private:
	TArray<uint32> ProcessIDs;
	bool IsAnyProcessIdSet;

	void Callback(uint32 newProcessId)
	{
		IsAnyProcessIdSet = true;
		ProcessIDs.Add(newProcessId);
	}
};

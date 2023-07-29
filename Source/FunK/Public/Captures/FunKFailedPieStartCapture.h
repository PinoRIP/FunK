// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct FFunKFailedPieStartCapture
{
public:
	FFunKFailedPieStartCapture()
	{
		IsAnyPieSessionEnded = true;
		FEditorDelegates::EndPIE.AddRaw(this, &FFunKFailedPieStartCapture::OnEndPIE);
	}

	~FFunKFailedPieStartCapture()
	{
		FEditorDelegates::EndPIE.RemoveAll(this);
	}

	bool CanProceed() const { return IsAnyPieSessionEnded; }

	void OnEndPIE(const bool bInSimulateInEditor)
	{
		IsAnyPieSessionEnded = false;
	}
	
private:
	bool IsAnyPieSessionEnded;
};

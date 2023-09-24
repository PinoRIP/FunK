// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropertyEditor/Public/IDetailCustomization.h"


struct FFunKActorSingleScenarioSetup;
struct FFunKActorScenarioAlsoSetup;
class IDetailGroup;
struct FFunKActorScenarioMultiplayerSetup;
class IDetailCategoryBuilder;

class FFunKActorScenarioScenarioDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	static void MakeScenarioMultiplayer(const FText& Text, IDetailGroup& Group, FFunKActorScenarioMultiplayerSetup& Setup);
	static void MakeSingleScenario(const FText& Text, IDetailGroup& Group, FFunKActorSingleScenarioSetup& Setup);
	static void MakeAlso(IDetailGroup& Group, FFunKActorScenarioAlsoSetup& Setup);
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "FFunKActorScenarioScenarioDetails.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "FunKLogging.h"
#include "IDetailGroup.h"
#include "ActorScenario/FunKActorScenarioTest.h"

#define LOCTEXT_NAMESPACE "FunKActorScenarioScenarioDetails"

TSharedRef<IDetailCustomization> FFunKActorScenarioScenarioDetails::MakeInstance()
{
	return MakeShareable(new FFunKActorScenarioScenarioDetails());
}

void FFunKActorScenarioScenarioDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Scenario", LOCTEXT("CatName", "Scenario Details"), ECategoryPriority::Important);

	AFunKActorScenarioTest* ActorScenarioTest = nullptr;
	for (TWeakObjectPtr<UObject> Object : ObjectsBeingCustomized)
	{
		ActorScenarioTest = Cast<AFunKActorScenarioTest>(Object);
		if(ActorScenarioTest) break;
	}
	
	if(!ActorScenarioTest)
	{
		UE_LOG(FunKLog, Warning, TEXT("FunKActorScenarioScenarioDetails has no AFunKActorScenarioTest target"))
		return;
	}

	auto setup = ActorScenarioTest->GetScenarioSetup();

	FText DisplayName = LOCTEXT("TestStandalone", "TestStandalone");
	
	Category.AddGroup("TestStandalone", DisplayName)
	.HeaderRow()
	.NameContent()
		[
			SNew(STextBlock)
			.Text(DisplayName)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(setup.TestStandalone ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			.IsEnabled(setup.TestStandaloneAvailable)
			.OnCheckStateChanged_Lambda([&setup](ECheckBoxState state)
			{
				setup.TestStandalone = state == ECheckBoxState::Checked;
			})
		];

	DisplayName = LOCTEXT("TestDedicatedServer", "TestDedicatedServer");
	MakeScenarioMultiplayer(DisplayName, Category.AddGroup("TestDedicatedServer", DisplayName), setup.TestDedicatedServer);
	
	DisplayName = LOCTEXT("TestDedicatedServer", "TestDedicatedServer");
	MakeScenarioMultiplayer(DisplayName, Category.AddGroup("TestListenServer", DisplayName), setup.TestListenServer);

	
}

void FFunKActorScenarioScenarioDetails::MakeScenarioMultiplayer(const FText& Text, IDetailGroup& Group, FFunKActorScenarioMultiplayerSetup& Setup)
{
	Group.HeaderRow()
	.NameContent()
		[
			SNew(STextBlock)
			.Text(Text)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(Setup.IsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			.IsEnabled(Setup.ClientToClient.IsAvailable || Setup.ClientToServer.IsAvailable || Setup.ServerToClient.IsAvailable)
			.OnCheckStateChanged_Lambda([&Setup](ECheckBoxState state)
			{
				Setup.IsActive = state == ECheckBoxState::Checked;
			})
		];

	FText DisplayName = LOCTEXT("ClientToClient", "ClientToClient");
	MakeSingleScenario(DisplayName, Group.AddGroup("ClientToClient", DisplayName), Setup.ClientToClient);

	DisplayName = LOCTEXT("ClientToServer", "ClientToServer");
	MakeSingleScenario(DisplayName, Group.AddGroup("ClientToServer", DisplayName), Setup.ClientToServer);
	
	DisplayName = LOCTEXT("ServerToClient", "ServerToClient");
	MakeSingleScenario(DisplayName, Group.AddGroup("ServerToClient", DisplayName), Setup.ServerToClient);
}

void FFunKActorScenarioScenarioDetails::MakeSingleScenario(const FText& Text, IDetailGroup& Group, FFunKActorSingleScenarioSetup& Setup)
{
	Group.HeaderRow()
	.NameContent()
		[
			SNew(STextBlock)
			.Text(Text)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SCheckBox)
			.IsChecked(Setup.IsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
			.IsEnabled(Setup.IsAvailable)
			.OnCheckStateChanged_Lambda([&Setup](ECheckBoxState state)
			{
				Setup.IsActive = state == ECheckBoxState::Checked;
			})
		];

	if(Setup.IsAvailable)
	{
		MakeAlso(Group.AddGroup("OnOpposition", LOCTEXT("OnOpposition", "OnOpposition")), Setup.OnOpposition);
		MakeAlso(Group.AddGroup("OnThird", LOCTEXT("OnThird", "OnThird")), Setup.OnThird);
	}
}

void FFunKActorScenarioScenarioDetails::MakeAlso(IDetailGroup& Group, FFunKActorScenarioAlsoSetup& Setup)
{
	Group.AddWidgetRow().NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("Arrange", "Arrange"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SCheckBox)
		.IsChecked(Setup.Arrange ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Lambda([&Setup](ECheckBoxState state)
		{
			Setup.Arrange = state == ECheckBoxState::Checked;
		})
	];

	Group.AddWidgetRow().NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("Act", "Act"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SCheckBox)
		.IsChecked(Setup.Act ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Lambda([&Setup](ECheckBoxState state)
		{
			Setup.Act = state == ECheckBoxState::Checked;
		})
	];

	Group.AddWidgetRow().NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("Assert", "Assert"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	[
		SNew(SCheckBox)
		.IsChecked(Setup.Assert ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Lambda([&Setup](ECheckBoxState state)
		{
			Setup.Assert = state == ECheckBoxState::Checked;
		})
	];
}

#undef LOCTEXT_NAMESPACE

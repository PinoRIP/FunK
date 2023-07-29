// Fill out your copyright notice in the Description page of Project Settings.


#include "FunKFunctionalTest.h"

#include "FunK.h"


// Sets default values
AFunKFunctionalTest::AFunKFunctionalTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFunKFunctionalTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFunKFunctionalTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFunKFunctionalTest::BuildTestRegistry(FString& append) const
{
	// Only include enabled tests in the list of functional tests to run.
	if (IsEnabled && (RunInStandaloneMode || RunInDedicatedServerMode || RunInListenServerMode))
	{
		append.Append(GetActorLabel() + TEXT("|") +
			(RunInStandaloneMode ? FFunKModule::FunkStandaloneParameter : TEXT("")) +
			(RunInDedicatedServerMode ? FFunKModule::FunkListenParameter : TEXT("")) +
			(RunInListenServerMode ? FFunKModule::FunkDedicatedParameter : TEXT("")) +
			":" + GetName()
		);
		append.Append(TEXT(";"));
	}
}

void AFunKFunctionalTest::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	Super::GetAssetRegistryTags(OutTags);

	if(IsPackageExternal() && IsEnabled)
	{
		FString TestActor;
		BuildTestRegistry(TestActor);
		const FName TestCategory = IsEditorOnlyObject(this) ? FFunKModule::FunkEditorOnlyTestWorldTag : FFunKModule::FunkTestWorldTag;
		OutTags.Add(UObject::FAssetRegistryTag(TestCategory, TestActor, UObject::FAssetRegistryTag::TT_Hidden));
	}
}


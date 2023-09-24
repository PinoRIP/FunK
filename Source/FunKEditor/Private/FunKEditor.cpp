// Fill out your copyright notice in the Description page of Project Settings.

#include "FunKEditor.h"

#include "FFunKActorScenarioScenarioDetails.h"
#include "ActorScenario/FunKActorScenarioTest.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FFunKEditor"

void FFunKEditor::StartupModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomClassLayout(AFunKActorScenarioTest::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FFunKActorScenarioScenarioDetails::MakeInstance));
}

void FFunKEditor::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFunKEditor, FunKEditor);
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_FunKActorScenarioLocalOwnership.generated.h"

class AFunKActorScenarioTest;

UCLASS()
class FUNKEDITOR_API UK2Node_FunKActorScenarioLocalOwnership : public UK2Node
{
	GENERATED_BODY()

	
public:
	// Constructor
	UK2Node_FunKActorScenarioLocalOwnership(const FObjectInitializer& ObjectInitializer);

	// UK2Node interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	void SetupOutputPin(UEdGraphPin* SequenceOutput, FName PinName, FName OwnershipCheckFunctionName, FKismetCompilerContext& CompilerContext, UEdGraph* EdGraph);
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsNodePure() const override { return false; }
	// End of UK2Node interface

	virtual FText GetMenuCategory() const override;
};

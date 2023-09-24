#include "K2Node_FunKActorScenarioLocalOwnership.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "ActorScenario/FunKActorScenarioTest.h"
#include "Internal/ActorScenario/FunKActorScenarioLocalOwnershipCheckFunctions.h"

#define LOCTEXT_NAMESPACE "FunKActorScenarioLocalOwnership"

namespace FunKActorScenarioLocalOwnershipPinNames
{
	static const FName InputActorScenario(TEXT("ActorScenario"));
	static const FName OutputNone(TEXT("None"));
	static const FName OutputAppositionPlayer(TEXT("AppositionPlayer"));
	static const FName OutputOppositionPlayer(TEXT("OppositionPlayer"));
	static const FName OutputAI(TEXT("AI"));
}

namespace FunKActorScenarioLocalOwnershipCheckFunctions
{
	static const FName IsOwnershipNone(GET_FUNCTION_NAME_CHECKED(UFunKActorScenarioLocalOwnershipCheckFunctions, IsOwnershipNone));
	static const FName IsOwnershipAppositionPlayer(GET_FUNCTION_NAME_CHECKED(UFunKActorScenarioLocalOwnershipCheckFunctions, IsOwnershipAppositionPlayer));
	static const FName IsOwnershipOppositionPlayer(GET_FUNCTION_NAME_CHECKED(UFunKActorScenarioLocalOwnershipCheckFunctions, IsOwnershipOppositionPlayer));
	static const FName IsOwnershipAI(GET_FUNCTION_NAME_CHECKED(UFunKActorScenarioLocalOwnershipCheckFunctions, IsOwnershipAI));
}

UK2Node_FunKActorScenarioLocalOwnership::UK2Node_FunKActorScenarioLocalOwnership(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_FunKActorScenarioLocalOwnership::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("Input"));
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AFunKActorScenarioTest::StaticClass(), FunKActorScenarioLocalOwnershipPinNames::InputActorScenario);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FunKActorScenarioLocalOwnershipPinNames::OutputNone);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FunKActorScenarioLocalOwnershipPinNames::OutputAppositionPlayer);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FunKActorScenarioLocalOwnershipPinNames::OutputOppositionPlayer);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FunKActorScenarioLocalOwnershipPinNames::OutputAI);

	Super::AllocateDefaultPins();
}

FText UK2Node_FunKActorScenarioLocalOwnership::GetTooltipText() const
{
    return FText::FromString(TEXT("FunKActorScenarioLocalOwnership"));
}

FText UK2Node_FunKActorScenarioLocalOwnership::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(TEXT("FunKActorScenarioLocalOwnership"));
}

void UK2Node_FunKActorScenarioLocalOwnership::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_FunKActorScenarioLocalOwnership::SetupOutputPin(UEdGraphPin* SequenceOutput, FName PinName, FName OwnershipCheckFunctionName, FKismetCompilerContext& CompilerContext, UEdGraph* EdGraph)
{
	if(!SequenceOutput)
	{
		CompilerContext.MessageLog.Error(TEXT("Invalid SequenceOutput!"));
		return;
	}

	UEdGraphPin* Pin = FindPin(PinName);
	if(!Pin)
	{
		CompilerContext.MessageLog.Error(TEXT("Invalid PinName!"));
		return;
	}
	
	UClass* Class = UFunKActorScenarioLocalOwnershipCheckFunctions::StaticClass();
	UFunction* OwnershipCheckFunction = Class->FindFunctionByName(OwnershipCheckFunctionName);
	if(!OwnershipCheckFunction)
	{
		CompilerContext.MessageLog.Error(TEXT("Invalid OwnershipCheckFunctionName!"));
		return;
	}

	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, EdGraph);
	CallFunction->SetFromFunction(OwnershipCheckFunction);
	CallFunction->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallFunction, this);

	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(FunKActorScenarioLocalOwnershipPinNames::InputActorScenario, EGPD_Input), *CallFunction->FindPinChecked(TEXT("Test")));
	CompilerContext.MovePinLinksToIntermediate(*SequenceOutput, *CallFunction->GetExecPin());

	UK2Node_IfThenElse* ConditionNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, EdGraph);
	ConditionNode->AllocateDefaultPins();

	CompilerContext.MovePinLinksToIntermediate(*CallFunction->GetReturnValuePin(), *ConditionNode->GetConditionPin());
	CompilerContext.MovePinLinksToIntermediate(*CallFunction->GetThenPin(), *ConditionNode->GetExecPin());
}

void UK2Node_FunKActorScenarioLocalOwnership::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
	return;

	UEdGraphPin* TestInputPin = FindPin(FunKActorScenarioLocalOwnershipPinNames::InputActorScenario, EGPD_Input);
	if (TestInputPin == nullptr || TestInputPin->LinkedTo.Num() <= 0)
	{
		CompilerContext.MessageLog.Error(TEXT("must have a valid ActorScenario pin"));
		return;
	}

	UK2Node_ExecutionSequence* SequenceNode = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	SequenceNode->AllocateDefaultPins();
	SequenceNode->AddInputPin();
	SequenceNode->AddInputPin();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(SequenceNode, this);

	TArray<UEdGraphPin*> OutputPins;
	for (UEdGraphPin* Pin : SequenceNode->Pins)
	{
		if(Pin->Direction == EGPD_Output)
		{
			OutputPins.Add(Pin);
		}
	}

	if(OutputPins.Num() != 4)
	{
		CompilerContext.MessageLog.Error(TEXT("Invalid amount of sequence output pins!"), this);
		return;
	}

	SetupOutputPin(OutputPins[0], FunKActorScenarioLocalOwnershipPinNames::OutputNone, FunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipNone, CompilerContext, SourceGraph);
	SetupOutputPin(OutputPins[1], FunKActorScenarioLocalOwnershipPinNames::OutputAppositionPlayer, FunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipAppositionPlayer, CompilerContext, SourceGraph);
	SetupOutputPin(OutputPins[2], FunKActorScenarioLocalOwnershipPinNames::OutputOppositionPlayer, FunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipOppositionPlayer, CompilerContext, SourceGraph);
	SetupOutputPin(OutputPins[3], FunKActorScenarioLocalOwnershipPinNames::OutputAI, FunKActorScenarioLocalOwnershipCheckFunctions::IsOwnershipAI, CompilerContext, SourceGraph);
}

FText UK2Node_FunKActorScenarioLocalOwnership::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

#undef LOCTEXT_NAMESPACE
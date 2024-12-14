// Copyright @ Christian Reichel

#include "GraphEditor/CustomNode.h"
#include "GraphEditor/CustomGraphNode.h"
#include "EdGraph/EdGraph.h"

UCustomNode::UCustomNode()
{
	NodeName = TEXT("Secret Node");
}

void UCustomNode::PostPasteNode()
{
	Super::PostPasteNode();
    
	// Clear existing pins
	Pins.Empty();
    
	// Recreate pins with fresh GUIDs
	AllocateDefaultPins();
}

void UCustomNode::AllocateDefaultPins()
{
	// Clear any existing pins first
	Pins.Empty();
    
	// Create single pins that can act as both input and output
	CreatePin(EGPD_Input, TEXT("Default"), TEXT("Top"));
	CreatePin(EGPD_Input, TEXT("Default"), TEXT("Right"));
	CreatePin(EGPD_Input, TEXT("Default"), TEXT("Bottom"));
	CreatePin(EGPD_Input, TEXT("Default"), TEXT("Left"));
}


FText UCustomNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Castle Node"));
}

FLinearColor UCustomNode::GetNodeTitleColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f); // Light gray
}

TSharedPtr<SGraphNode> UCustomNode::CreateVisualWidget()
{
	return SNew(SCustomGraphNode, this);
}

void UCustomNode::PostEditUndo()
{
	Super::PostEditUndo();
	if (UEdGraph* Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}

void UCustomNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (UEdGraph* Graph = GetGraph())
	{
		Graph->NotifyGraphChanged();
	}
}
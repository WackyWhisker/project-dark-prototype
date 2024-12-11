// Copyright @ Christian Reichel

#include "GraphEditor/CustomNode.h"
#include "GraphEditor/CustomGraphNode.h"
#include "EdGraph/EdGraph.h"

UCustomNode::UCustomNode()
{
	NodeName = TEXT("Secret Node");
}

void UCustomNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
    
	// Create an input pin
	CreatePin(EGPD_Input, TEXT("Default"), TEXT("Input"));
    
	// Create an output pin
	CreatePin(EGPD_Output, TEXT("Default"), TEXT("Output"));
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
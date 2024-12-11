// Copyright @ Christian Reichel


#include "GraphEditor/CustomNode.h"
#include "GraphEditor/CustomGraphNode.h"

UCustomNode::UCustomNode()
{
	NodeName = TEXT("New Node");
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
	return FText::FromString(TEXT("Custom Node"));
}

FLinearColor UCustomNode::GetNodeTitleColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f); // Light gray
}

TSharedPtr<SGraphNode> UCustomNode::CreateVisualWidget()
{
	return SNew(SCustomGraphNode, this);
}

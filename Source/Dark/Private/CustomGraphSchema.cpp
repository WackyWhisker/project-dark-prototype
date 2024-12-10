// Copyright @ Christian Reichel


#include "CustomGraphSchema.h"
#include "GraphEditor/CustomNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuSection.h"
#include "EdGraphNode_Comment.h"

void UCustomGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
    // Create action to add new node
    const FText MenuDesc = FText::FromString("Add Custom Node");
    const FText ToolTip = FText::FromString("Add a new custom node to the graph");
    
    TSharedPtr<FEdGraphSchemaAction_NewNode> Action(new FEdGraphSchemaAction_NewNode(
        FText::FromString("Custom Nodes"),
        MenuDesc,
        ToolTip,
        0));
        
    UCustomNode* NodeTemplate = NewObject<UCustomNode>();
    Action->NodeTemplate = NodeTemplate;
    
    ContextMenuBuilder.AddAction(Action);
}

void UCustomGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    if (Context->Node)
    {
        FToolMenuSection& Section = Menu->AddSection("CustomNodeActions", FText::FromString("Node Actions"));
        
        // Add the basic operations
        Section.AddMenuEntry(FGenericCommands::Get().Delete);
        Section.AddMenuEntry(FGenericCommands::Get().Cut);
        Section.AddMenuEntry(FGenericCommands::Get().Copy);
        Section.AddMenuEntry(FGenericCommands::Get().Paste);
        Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
        Section.AddMenuEntry(FGenericCommands::Get().SelectAll);
    }
}

const FPinConnectionResponse UCustomGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
    // Basic validation
    if (!A || !B)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid pins"));
    }

    // Prevent connecting to self
    if (A->GetOwningNode() == B->GetOwningNode())
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect a node to itself"));
    }

    // For now, allow all other connections between different nodes
    return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}
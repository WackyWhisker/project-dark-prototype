// Copyright @ Christian Reichel


#include "CastleRooms/CastleRoomsSchema.h"
#include "CastleRooms/CastleRoomsNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"

void UCastleRoomsSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
    // Create action to add new room node
    const FText MenuDesc = FText::FromString("Add Room");
    const FText ToolTip = FText::FromString("Add a new room to the castle layout");

    TSharedPtr<FEdGraphSchemaAction_NewNode> Action(new FEdGraphSchemaAction_NewNode(
        FText::FromString("Castle Rooms"),
        MenuDesc,
        ToolTip,
        0));

    UCastleRoomsNode* NodeTemplate = NewObject<UCastleRoomsNode>();
    Action->NodeTemplate = NodeTemplate;
    ContextMenuBuilder.AddAction(Action);
}

void UCastleRoomsSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    // Add any node-specific context menu items
    if (Context->Node)
    {
        // We'll add specific actions based on node type later
        FToolMenuSection& Section = Menu->AddSection("CastleRoomsNodeActions", FText::FromString("Node Actions"));
    }
}

const FPinConnectionResponse UCastleRoomsSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
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
    // We'll add more specific rules later
    return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

UEdGraphNode* UCastleRoomsSchema::CreateNode(UEdGraph* Graph, const FVector2D& Location, TSubclassOf<UEdGraphNode> NodeClass) const
{
    if (!Graph || !NodeClass)
    {
        return nullptr;
    }

    UEdGraphNode* NewNode = NewObject<UEdGraphNode>(Graph, NodeClass);
    NewNode->CreateNewGuid();
    NewNode->PostPlacedNewNode();
    NewNode->NodePosX = Location.X;
    NewNode->NodePosY = Location.Y;
    
    // Add to graph
    Graph->AddNode(NewNode);
    
    return NewNode;
}

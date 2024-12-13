// Copyright @ Christian Reichel

#include "GraphEditor/CustomGraphSchema.h"
#include "GraphEditor/CustomNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenuSection.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditor/CustomConnectionDrawingPolicy.h"

void UCustomGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
    // Create action to add new node
    const FText MenuDesc = FText::FromString("Add Castle Node");
    const FText ToolTip = FText::FromString("Add a new Castle node to the graph");
    
    TSharedPtr<FEdGraphSchemaAction_NewNode> Action(new FEdGraphSchemaAction_NewNode(
        FText::FromString("Castle Nodes"),
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

    // Allow any pin to connect to any other pin regardless of direction
    // This effectively makes all pins bidirectional
    return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UCustomGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
    const FPinConnectionResponse Response = CanCreateConnection(A, B);
    bool bModified = false;

    if (Response.Response == CONNECT_RESPONSE_MAKE)
    {
        // When making the connection, determine which pin should be input and which should be output
        if (A->LinkedTo.Num() == 0)  // If A has no connections, keep its current direction
        {
            if (B->LinkedTo.Num() > 0)  // If B is already connected, adapt A's direction
            {
                A->Direction = (B->Direction == EGPD_Input) ? EGPD_Output : EGPD_Input;
            }
            // Otherwise keep A's direction and adapt B
            B->Direction = (A->Direction == EGPD_Input) ? EGPD_Output : EGPD_Input;
        }
        else  // A is already connected, adapt B's direction
        {
            B->Direction = (A->Direction == EGPD_Input) ? EGPD_Output : EGPD_Input;
        }

        A->Modify();
        B->Modify();
        A->MakeLinkTo(B);
        bModified = true;
    }

    return bModified;
}

void UCustomGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
    Super::BreakNodeLinks(TargetNode);
}

void UCustomGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
    Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

void UCustomGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
    Super::BreakSinglePinLink(SourcePin, TargetPin);
}

FLinearColor UCustomGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
    // Use a distinctive color for our bidirectional pins
    return FLinearColor(0.8f, 0.8f, 0.2f); // Gold-ish color to match the pin visuals
}

FConnectionDrawingPolicy* UCustomGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
    return new FCustomConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}
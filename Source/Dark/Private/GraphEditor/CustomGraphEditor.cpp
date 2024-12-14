// Copyright @ Christian Reichel

#include "GraphEditor/CustomGraphEditor.h"
#include "GraphEditor/CustomGraphAsset.h"
#include "GraphEditor/CustomNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Commands/UICommandList.h"
#include "GraphEditor.h"
#include "SNodePanel.h"
#include "Modules/ModuleManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EdGraphUtilities.h"

void SCustomGraphEditor::Construct(const FArguments& InArgs, UCustomGraphAsset* InGraphAsset)
{
    GraphAsset = InGraphAsset;

    // Create the appearance info for the graph editor
    FGraphAppearanceInfo AppearanceInfo;
    AppearanceInfo.CornerText = FText::FromString("Castle Graph");

    // Initialize command list
    GraphEditorCommands = MakeShareable(new FUICommandList);

    // Bind commands
    BindCommands();

    // Create the graph editor widget
    GraphEditorWidget = SNew(SGraphEditor)
        .AdditionalCommands(GraphEditorCommands)
        .IsEditable(true)
        .Appearance(AppearanceInfo)
        .GraphToEdit(GraphAsset->EdGraph);

    // Set this widget to be the graph editor
    ChildSlot
    [
        GraphEditorWidget.ToSharedRef()
    ];
}

void SCustomGraphEditor::BindCommands()
{
    // First register the standard commands
    const FGenericCommands& GenericCommands = FGenericCommands::Get();

    // Delete
    GraphEditorCommands->MapAction(
        GenericCommands.Delete,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnDeleteNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanDeleteNode)
    );

    // Cut
    GraphEditorCommands->MapAction(
        GenericCommands.Cut,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnCutNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanCutNode)
    );

    // Copy
    GraphEditorCommands->MapAction(
        GenericCommands.Copy,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnCopyNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanCopyNode)
    );

    // Paste
    GraphEditorCommands->MapAction(
        GenericCommands.Paste,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnPasteNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanPasteNode)
    );

    // Duplicate
    GraphEditorCommands->MapAction(
        GenericCommands.Duplicate,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnDuplicateNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanDuplicateNode)
    );

    // Select All
    GraphEditorCommands->MapAction(
        GenericCommands.SelectAll,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnSelectAllNodes),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanSelectAllNodes)
    );
}

void SCustomGraphEditor::LogCommandExecution(const FString& CommandName) const
{
    UE_LOG(LogTemp, Display, TEXT("Executing command: %s"), *CommandName);
}

void SCustomGraphEditor::OnDeleteNode()
{
    LogCommandExecution(TEXT("Delete"));
    const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
    GraphEditorWidget->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
    {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
        {
            Node->Modify();
            Node->DestroyNode();
        }
    }

    // Notify graph changed
    if (GraphAsset.IsValid() && GraphAsset->EdGraph)
    {
        GraphAsset->EdGraph->NotifyGraphChanged();
    }
}

void SCustomGraphEditor::OnCutNode()
{
    LogCommandExecution(TEXT("Cut"));
    // First copy
    OnCopyNode();

    // Then delete
    const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
    GraphEditorWidget->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
    {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
        {
            Node->Modify();
            Node->DestroyNode();
        }
    }

    // Notify graph changed
    if (GraphAsset.IsValid() && GraphAsset->EdGraph)
    {
        GraphAsset->EdGraph->NotifyGraphChanged();
    }
}

void SCustomGraphEditor::OnCopyNode()
{
    LogCommandExecution(TEXT("Copy"));
    const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
    if (SelectedNodes.Num() > 0)
    {
        // Store the average position of copied nodes
        FVector2D AveragePosition(0.0f, 0.0f);
        for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
        {
            if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
            {
                AveragePosition.X += Node->NodePosX;
                AveragePosition.Y += Node->NodePosY;
            }
        }
        AveragePosition /= SelectedNodes.Num();
        LastCopyLocation = AveragePosition;

        FString ExportedText;
        FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
        FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
    }
}

void SCustomGraphEditor::OnPasteNode()
{
    LogCommandExecution(TEXT("Paste"));
    FString TextToImport;
    FPlatformApplicationMisc::ClipboardPaste(TextToImport);

    if (!TextToImport.IsEmpty() && GraphAsset.IsValid())
    {
        const FVector2D CursorPosition = GraphEditorWidget->GetPasteLocation();
        
        UE_LOG(LogTemp, Warning, TEXT("Starting paste operation at cursor position: %s"), *CursorPosition.ToString());

        TSet<UEdGraphNode*> PastedNodes;
        FEdGraphUtilities::ImportNodesFromText(GraphAsset->EdGraph, TextToImport, PastedNodes);

        // Calculate offset based on cursor position
        const float OffsetAmount = 100.0f;
        FVector2D Offset = FVector2D(OffsetAmount, OffsetAmount);

        // Apply the offset to all pasted nodes
        for (UEdGraphNode* Node : PastedNodes)
        {
            UE_LOG(LogTemp, Warning, TEXT("Pasted Node: %s (%p)"), *Node->GetName(), Node);
            
            Node->NodePosX += Offset.X;
            Node->NodePosY += Offset.Y;

            // Log information about each pin
            for (UEdGraphPin* Pin : Node->Pins)
            {
                UE_LOG(LogTemp, Warning, TEXT("  Pin: %s, Direction: %d"), 
                    *Pin->PinName.ToString(),
                    static_cast<int32>(Pin->Direction));
            }

            Node->ReconstructNode();
        }

        // Clear any existing selection and select the newly pasted nodes
        GraphEditorWidget->ClearSelectionSet();
        for (UEdGraphNode* Node : PastedNodes)
        {
            GraphEditorWidget->SetNodeSelection(Node, true);
        }

        // Notify graph changed
        if (GraphAsset->EdGraph)
        {
            GraphAsset->EdGraph->NotifyGraphChanged();
        }
    }
}

void SCustomGraphEditor::OnSelectAllNodes()
{
    LogCommandExecution(TEXT("Select All"));
    if (GraphEditorWidget.IsValid())
    {
        GraphEditorWidget->SelectAllNodes();
    }
}

void SCustomGraphEditor::OnDuplicateNode()
{
    LogCommandExecution(TEXT("Duplicate"));
    OnCopyNode();
    OnPasteNode();
}

bool SCustomGraphEditor::CanDeleteNode() const
{
    return GraphEditorWidget.IsValid() && GraphEditorWidget->GetSelectedNodes().Num() > 0;
}

bool SCustomGraphEditor::CanCutNode() const
{
    return CanCopyNode();
}

bool SCustomGraphEditor::CanCopyNode() const
{
    return GraphEditorWidget.IsValid() && GraphEditorWidget->GetSelectedNodes().Num() > 0;
}

bool SCustomGraphEditor::CanPasteNode() const
{
    FString ClipboardContent;
    FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
    return !ClipboardContent.IsEmpty() && GraphAsset.IsValid();
}

bool SCustomGraphEditor::CanDuplicateNode() const
{
    return CanCopyNode();
}

bool SCustomGraphEditor::CanSelectAllNodes() const
{
    return GraphEditorWidget.IsValid() && GraphAsset.IsValid();
}
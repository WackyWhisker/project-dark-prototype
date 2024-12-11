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
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "EdGraphUtilities.h"

void SCustomGraphEditor::Construct(const FArguments& InArgs, UCustomGraphAsset* InGraphAsset)
{
    GraphAsset = InGraphAsset;

    // Initialize command list
    GraphEditorCommands = MakeShareable(new FUICommandList);

    // Bind commands
    BindCommands();

    // Create the appearance info for the graph editor
    FGraphAppearanceInfo AppearanceInfo;
    AppearanceInfo.CornerText = FText::FromString("Custom Graph");

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

    GraphEditorCommands->MapAction(
        GenericCommands.Delete,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnDeleteNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanDeleteNode)
    );

    GraphEditorCommands->MapAction(
        GenericCommands.Copy,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnCopyNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanCopyNode)
    );

    GraphEditorCommands->MapAction(
        GenericCommands.Paste,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnPasteNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanPasteNode)
    );

    GraphEditorCommands->MapAction(
        GenericCommands.Duplicate,
        FExecuteAction::CreateSP(this, &SCustomGraphEditor::OnDuplicateNode),
        FCanExecuteAction::CreateSP(this, &SCustomGraphEditor::CanDuplicateNode)
    );
}

void SCustomGraphEditor::OnDeleteNode()
{
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
}

void SCustomGraphEditor::OnCopyNode()
{
    // Exports the selected nodes to text
    const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
    FString ExportedText;
    FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
    FGenericPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

void SCustomGraphEditor::OnPasteNode()
{
    // Grab the text to paste from the clipboard.
    FString TextToImport;
    FGenericPlatformApplicationMisc::ClipboardPaste(TextToImport);

    // Import the nodes
    const FVector2D PasteLocation = GraphEditorWidget->GetPasteLocation();
    
    // Create a set to store the pasted nodes
    TSet<UEdGraphNode*> PastedNodes;
    
    // Import the nodes with the correct parameters
    FEdGraphUtilities::ImportNodesFromText(GraphAsset->EdGraph, TextToImport, PastedNodes);
    
    // Optionally, you can adjust the location of pasted nodes
    for (UEdGraphNode* Node : PastedNodes)
    {
        Node->NodePosX += PasteLocation.X;
        Node->NodePosY += PasteLocation.Y;
    }
}

void SCustomGraphEditor::OnDuplicateNode()
{
    // Copy and paste in one go
    OnCopyNode();
    OnPasteNode();
}

bool SCustomGraphEditor::CanDeleteNode() const
{
    return GraphEditorWidget->GetSelectedNodes().Num() > 0;
}

bool SCustomGraphEditor::CanCopyNode() const
{
    return GraphEditorWidget->GetSelectedNodes().Num() > 0;
}

bool SCustomGraphEditor::CanPasteNode() const
{
    FString ClipboardContent;
    FGenericPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
    return !ClipboardContent.IsEmpty();
}

bool SCustomGraphEditor::CanDuplicateNode() const
{
    return CanCopyNode();
}
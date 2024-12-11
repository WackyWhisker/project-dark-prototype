// Copyright @ Christian Reichel


#include "GraphEditor/CustomGraphEditor.h"
#include "GraphEditor/CustomGraphAsset.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"

void SCustomGraphEditor::Construct(const FArguments& InArgs, UCustomGraphAsset* InGraphAsset)
{
	GraphAsset = InGraphAsset;

	// Initialize command list
	GraphEditorCommands = MakeShareable(new FUICommandList);

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
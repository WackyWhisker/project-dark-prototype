// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "Widgets/SCompoundWidget.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/Commands.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"

class UCustomGraphAsset;

class SCustomGraphEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCustomGraphEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UCustomGraphAsset* InGraphAsset);

private:
	// Pointer to the graph asset being edited
	TWeakObjectPtr<UCustomGraphAsset> GraphAsset;

	// The actual graph editor widget
	TSharedPtr<SGraphEditor> GraphEditorWidget;

	// Commands handled by this editor
	TSharedPtr<FUICommandList> GraphEditorCommands;

	// Command functions
	void BindCommands();
    
	void OnDeleteNode();
	void OnCopyNode();
	void OnPasteNode();
	void OnDuplicateNode();

	bool CanDeleteNode() const;
	bool CanCopyNode() const;
	bool CanPasteNode() const;
	bool CanDuplicateNode() const;
};
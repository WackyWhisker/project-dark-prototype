// Copyright @ Christian Reichel


#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "Widgets/SCompoundWidget.h"

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
};
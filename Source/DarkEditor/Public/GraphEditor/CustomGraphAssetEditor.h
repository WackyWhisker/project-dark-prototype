// Copyright @ Christian Reichel

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class UCustomGraphAsset;
class SCustomGraphEditor;

class FCustomGraphAssetEditor : public FAssetEditorToolkit
{
public:
	FCustomGraphAssetEditor();
	virtual ~FCustomGraphAssetEditor();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	// End of FAssetEditorToolkit interface

	// Initialize the editor
	void InitCustomGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCustomGraphAsset* InCustomGraph);

private:
	// The graph asset being edited
	UCustomGraphAsset* EditedGraph;

	// Spawns the graph editor tab
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);

	static const FName GraphCanvasTabId;
};
#endif
// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "GraphEditor.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCastleRooms, Log, All);

class FCastleRoomsGraphEditor : public FWorkflowCentricApplication
{
public:
	// Public Methods
	void InitCastleRoomsEditor(const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit);

	// WorkflowCentricApplication interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void SaveAsset_Execute() override;

public:
	// Public Properties

protected:
	// Protected Methods
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);

protected:
	// Protected Properties

private:
	// Private Methods
	static const FName GraphCanvasTabId;

private:
	// Private Properties
	TSharedPtr<SGraphEditor> GraphEditorPtr;
	UEdGraph* GraphObj;
};
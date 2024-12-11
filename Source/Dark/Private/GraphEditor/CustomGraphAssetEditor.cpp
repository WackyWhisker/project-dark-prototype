// Copyright @ Christian Reichel


#include "GraphEditor/CustomGraphAssetEditor.h"
#include "GraphEditor/CustomGraphAsset.h"
#include "GraphEditor/CustomGraphEditor.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

const FName FCustomGraphAssetEditor::GraphCanvasTabId(TEXT("GraphCanvas"));

FCustomGraphAssetEditor::FCustomGraphAssetEditor()
{
    EditedGraph = nullptr;
}

FCustomGraphAssetEditor::~FCustomGraphAssetEditor()
{
}

void FCustomGraphAssetEditor::InitCustomGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCustomGraphAsset* InCustomGraph)
{
    EditedGraph = InCustomGraph;

    // Create the layout
    const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("CustomGraphAssetEditor_Layout")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab(GraphCanvasTabId, ETabState::OpenedTab)
            )
        );

    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = true;
    FAssetEditorToolkit::InitAssetEditor(
        Mode,
        InitToolkitHost,
        FName("CustomGraphEditorApp"),
        Layout,
        bCreateDefaultStandaloneMenu,
        bCreateDefaultToolbar,
        EditedGraph
    );
}

void FCustomGraphAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FCustomGraphAssetEditor::SpawnTab_GraphCanvas))
        .SetDisplayName(FText::FromString("Graph"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FCustomGraphAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
    InTabManager->UnregisterTabSpawner(GraphCanvasTabId);
}

FName FCustomGraphAssetEditor::GetToolkitFName() const
{
    return FName("CustomGraphEditor");
}

FText FCustomGraphAssetEditor::GetBaseToolkitName() const
{
    return FText::FromString("Custom Graph Editor");
}

FString FCustomGraphAssetEditor::GetWorldCentricTabPrefix() const
{
    return TEXT("CustomGraphEditor");
}

FLinearColor FCustomGraphAssetEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

TSharedRef<SDockTab> FCustomGraphAssetEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == GraphCanvasTabId);

    return SNew(SDockTab)
        [
            SNew(SCustomGraphEditor, EditedGraph)
        ];
}
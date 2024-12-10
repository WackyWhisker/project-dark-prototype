// Copyright @ Christian Reichel

#include "CastleRooms/CastleRoomsGraphEditor.h"
#include "FileHelpers.h"
#include "CastleRooms/CastleRoomsGraph.h"
#include "CastleRooms/CastleRoomsSchema.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "EdGraph/EdGraph.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif

#define LOCTEXT_NAMESPACE "CastleRoomsEditor"

#define CASTLEROOMS_LOG(Verbosity, Format, ...) \
UE_LOG(LogTemp, Verbosity, TEXT(Format), ##__VA_ARGS__)

static const FName CastleRoomsEditorAppName(TEXT("CastleRoomsEditorApp"));
const FName FCastleRoomsGraphEditor::GraphCanvasTabId(TEXT("CastleRoomsEditor_GraphCanvas"));

void FCastleRoomsGraphEditor::InitCastleRoomsEditor(const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
   CASTLEROOMS_LOG(Warning, "Starting InitCastleRoomsEditor with ObjectToEdit: {0}", ObjectToEdit ? *ObjectToEdit->GetName() : TEXT("nullptr"));

   GraphObj = Cast<UEdGraph>(ObjectToEdit);
   if (!GraphObj)
   {
       CASTLEROOMS_LOG(Error, "Failed to cast ObjectToEdit to UEdGraph");
       return;
   }

   TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_CastleRoomsEditor")
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
       EToolkitMode::Standalone,
       InitToolkitHost,
       CastleRoomsEditorAppName,
       StandaloneDefaultLayout,
       bCreateDefaultToolbar,
       bCreateDefaultStandaloneMenu,
       ObjectToEdit);
}

void FCastleRoomsGraphEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
   FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

   InTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FCastleRoomsGraphEditor::SpawnTab_GraphCanvas))
       .SetDisplayName(FText::FromString("Graph Canvas"));
}

void FCastleRoomsGraphEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
   FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FName FCastleRoomsGraphEditor::GetToolkitFName() const
{
   return FName("CastleRoomsEditor");
}

FText FCastleRoomsGraphEditor::GetBaseToolkitName() const
{
   return FText::FromString("Castle Rooms Editor");
}

FString FCastleRoomsGraphEditor::GetWorldCentricTabPrefix() const
{
   return TEXT("CastleRoomsEditor");
}

FLinearColor FCastleRoomsGraphEditor::GetWorldCentricTabColorScale() const
{
   return FLinearColor(0.3f, 0.2f, 0.1f, 0.5f); // Brownish color for castle theme
}

TSharedRef<SDockTab> FCastleRoomsGraphEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
   GraphEditorPtr = SNew(SGraphEditor)
       .GraphToEdit(GraphObj);
   CASTLEROOMS_LOG(Warning, "SpawnTab_GraphCanvas called. GraphEditorPtr valid: {0}", GraphEditorPtr.IsValid() ? TEXT("true") : TEXT("false"));
   return SNew(SDockTab)
       [
           GraphEditorPtr.ToSharedRef()
       ];
}

void FCastleRoomsGraphEditor::SaveAsset_Execute()
{
   if (GraphObj)
   {
       TArray<UPackage*> PackagesToSave;
       PackagesToSave.Add(GraphObj->GetPackage());
       FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, false);
   }
}

#undef LOCTEXT_NAMESPACE
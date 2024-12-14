#include "DarkEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "GraphEditor/CustomGraphAssetTypeActions.h"

IMPLEMENT_MODULE(FDarkEditorModule, DarkEditor)

void FDarkEditorModule::StartupModule()
{
    // Register asset types
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
    // Create and register the asset type actions
    TSharedRef<IAssetTypeActions> Action = MakeShareable(new FCustomGraphAssetTypeActions);
    AssetTools.RegisterAssetTypeActions(Action);
}

void FDarkEditorModule::ShutdownModule()
{
    // Nothing needed for shutdown yet
}
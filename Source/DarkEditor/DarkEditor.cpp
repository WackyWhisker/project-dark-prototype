#include "DarkEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "GraphEditor/CustomGraphAssetTypeActions.h"

IMPLEMENT_MODULE(FDarkEditorModule, DarkEditor)

void FDarkEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("DarkEditor Module Starting"));
    
    // Register asset types
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
    // Create and register the asset type actions
    TSharedRef<IAssetTypeActions> Action = MakeShareable(new FCustomGraphAssetTypeActions);
    AssetTools.RegisterAssetTypeActions(Action);
    
    UE_LOG(LogTemp, Warning, TEXT("DarkEditor Module: Registered Asset Actions"));
}

void FDarkEditorModule::ShutdownModule()
{
    // Nothing needed for shutdown yet
}
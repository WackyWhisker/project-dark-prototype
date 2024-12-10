// Copyright @ Christian Reichel

#include "Dark.h"
#include "Modules/ModuleManager.h"
#include "GraphEditor/CustomGraphAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IAssetTools.h"

void FDarkGameModule::StartupModule()
{
	// Call parent implementation
	FDefaultGameModuleImpl::StartupModule();

	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
	// Create and register our custom asset type action
	TSharedPtr<FCustomGraphAssetTypeActions> ActionType = MakeShareable(new FCustomGraphAssetTypeActions());
	AssetTools.RegisterAssetTypeActions(ActionType.ToSharedRef());
    
	// Keep track of created action type for cleanup
	CreatedAssetTypeActions.Add(ActionType);
}

void FDarkGameModule::ShutdownModule()
{
	// Unregister asset types
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        
		// Unregister all actions we registered
		for (auto Action : CreatedAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();

	// Call parent implementation
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDarkGameModule, Dark, "Dark");
// Copyright @ Christian Reichel

#include "Dark.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "CastleRooms/CastleRoomsEditorFactory.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDarkModule, Dark, "Dark");

void FDarkModule::StartupModule()
{
	CastleRoomsAssetTypeActions = MakeShareable(new FCastleRoomsAssetTypeActions);
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(CastleRoomsAssetTypeActions.ToSharedRef());
}

void FDarkModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(CastleRoomsAssetTypeActions.ToSharedRef());
	}
	CastleRoomsAssetTypeActions.Reset();
}
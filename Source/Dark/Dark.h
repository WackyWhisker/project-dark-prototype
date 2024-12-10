// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCastleRoomsAssetTypeActions;

class FDarkModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FCastleRoomsAssetTypeActions> CastleRoomsAssetTypeActions;
};
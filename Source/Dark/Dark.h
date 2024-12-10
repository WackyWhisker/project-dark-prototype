// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class FDarkGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	// Track registered asset type actions for cleanup
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
};

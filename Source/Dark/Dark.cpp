// Copyright @ Christian Reichel

#include "Dark.h"
#include "Modules/ModuleManager.h"

void FDarkGameModule::StartupModule()
{
	// Call parent implementation
	FDefaultGameModuleImpl::StartupModule();
}

void FDarkGameModule::ShutdownModule()
{
	// Call parent implementation
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDarkGameModule, Dark, "Dark");
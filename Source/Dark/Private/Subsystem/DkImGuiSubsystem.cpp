// Copyright @ Christian Reichel


#include "Subsystem/DkImGuiSubsystem.h"
#include "ImGuiModule.h"

UE_DISABLE_OPTIMIZATION

void UDkImGuiSubsystem::Tick(float DeltaTime)
{
	
}

TStatId UDkImGuiSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDevGuiSubsystem, STATGROUP_Tickables);
}

void UDkImGuiSubsystem::ToggleImGuiInput()
{
	FImGuiModule::Get().GetProperties().ToggleInput();
}

UE_ENABLE_OPTIMIZATION
// Copyright @ Christian Reichel


#include "Subsystem/DkImGuiSubsystem.h"
#include "ImGuiModule.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"
#include "Character/DkCharacter.h"

UE_DISABLE_OPTIMIZATION

void UDkImGuiSubsystem::Tick(float DeltaTime)
{
	if (bShowDebugMenu)
	{
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::Begin("Project Dark - Debug Menu", &bShowDebugMenu, ImGuiWindowFlags_AlwaysAutoResize);
        //TODO expand into multiple categories
		if (ImGui::Checkbox("Grant Double Jump Ability", &bDoubleJumpAbilityGranted))
		{
			if (bDoubleJumpAbilityGranted)
			{
				UE_LOG(LogTemp, Warning, TEXT("Double Jump Ability Granted!"));
				if (OwnerCharacter)
				{
					OwnerCharacter->GrantAbilityByName("Double Jump");//name hard referenced in gameplay ablity data
					OwnerCharacter->RevokeAbilityByName("Jump");
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Double Jump Ability Revoked!"));
				if (OwnerCharacter)
				{
					OwnerCharacter->RevokeAbilityByName("Double Jump");
					OwnerCharacter->GrantAbilityByName("Jump");
				}
			}
		}
        
		ImGui::End();
	}
}

TStatId UDkImGuiSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDevGuiSubsystem, STATGROUP_Tickables);
}

void UDkImGuiSubsystem::ToggleImGuiInput()
{
	FImGuiModule::Get().GetProperties().ToggleInput();
}

void UDkImGuiSubsystem::ToggleDebugMenu(ADkCharacter* InOwnerCharacter)
{
	bShowDebugMenu = !bShowDebugMenu;
	OwnerCharacter = InOwnerCharacter;
	FImGuiModule::Get().GetProperties().SetInputEnabled(bShowDebugMenu);
}

UE_ENABLE_OPTIMIZATION
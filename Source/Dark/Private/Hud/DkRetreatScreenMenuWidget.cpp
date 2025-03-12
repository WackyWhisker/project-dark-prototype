// Copyright @ Christian Reichel

#include "Hud/DkRetreatScreenMenuWidget.h"
#include "Hud/DkHUD.h"
#include "Subsystem/DkGameStateSubsystem.h"

void UDkRetreatScreenMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
    
	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddUObject(this, &UDkRetreatScreenMenuWidget::HandleResumeClicked);
	}
        
	if (RetreatButton)
	{
		RetreatButton->OnClicked().AddUObject(this, &UDkRetreatScreenMenuWidget::HandleRetreatClicked);
	}
}

void UDkRetreatScreenMenuWidget::HandleResumeClicked()
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (ADkHUD* DkHUD = Cast<ADkHUD>(PC->GetHUD()))
		{
			if (DkHUD)
			{
				DkHUD->HandleToggleRetreatScreenMenu();
			}
		}
	}
}

void UDkRetreatScreenMenuWidget::HandleRetreatClicked()
{
	// First close the menu
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (ADkHUD* DkHUD = Cast<ADkHUD>(PC->GetHUD()))
		{
			if (DkHUD)
			{
				DkHUD->HandleToggleRetreatScreenMenu();
			}
		}
        
		// Then request retreat state change
		if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
		{
			GameStateSubsystem->RequestStateChange(EDkGameState::Retreat);
		}
	}
}
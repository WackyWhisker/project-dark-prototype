// Copyright @ Christian Reichel


#include "Hud/DkEndScreenMenuWidget.h"
#include "Hud/DkHUD.h"
#include "Kismet/GameplayStatics.h"

void UDkEndScreenMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddUObject(this, &UDkEndScreenMenuWidget::HandleResumeClicked);
	}
        
	if (RestartButton)
	{
		RestartButton->OnClicked().AddUObject(this, &UDkEndScreenMenuWidget::HandleRestartClicked);
	}
    
	if (QuitButton)
	{
		QuitButton->OnClicked().AddUObject(this, &UDkEndScreenMenuWidget::HandleQuitClicked);
	}
}

void UDkEndScreenMenuWidget::HandleResumeClicked()
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (ADkHUD* DkHUD = Cast<ADkHUD>(PC->GetHUD()))
		{
			if (DkHUD)
			{
				DkHUD->HandleToggleEndScreenMenu();
			}
		}
	}
}

void UDkEndScreenMenuWidget::HandleRestartClicked()
{
	// First toggle pause menu off
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (ADkHUD* DkHUD = Cast<ADkHUD>(PC->GetHUD()))
		{
			if (DkHUD)
			{
				DkHUD->HandleToggleEndScreenMenu();
			}
		}
		// Then restart level
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
	}
}

void UDkEndScreenMenuWidget::HandleQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}

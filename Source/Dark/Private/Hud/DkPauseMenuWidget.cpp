// Copyright @ Christian Reichel

#include "Hud/DkPauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/DkPlayerController.h"

void UDkPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddUObject(this, &UDkPauseMenuWidget::HandleResumeClicked);
	}
        
	if (RestartButton)
	{
		RestartButton->OnClicked().AddUObject(this, &UDkPauseMenuWidget::HandleRestartClicked);
	}
    
	if (QuitButton)
	{
		QuitButton->OnClicked().AddUObject(this, &UDkPauseMenuWidget::HandleQuitClicked);
	}
}

void UDkPauseMenuWidget::HandleResumeClicked()
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (FTogglePauseMenuSignature* PauseDelegate = PC->GetTogglePauseMenuDelegate())
		{
			PauseDelegate->Broadcast();
		}
	}
}

void UDkPauseMenuWidget::HandleRestartClicked()
{
	// First toggle pause menu off
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (FTogglePauseMenuSignature* PauseDelegate = PC->GetTogglePauseMenuDelegate())
		{
			PauseDelegate->Broadcast();
		}
        
		// Then restart level
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
	}
}

void UDkPauseMenuWidget::HandleQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
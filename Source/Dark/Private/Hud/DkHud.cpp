// Copyright @ Christian Reichel

#include "Hud/DkHUD.h"
#include "Hud/DkPauseMenuWidget.h"
#include "Player/DkPlayerController.h"

ADkHUD::ADkHUD()
{
}

void ADkHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create the pause menu widget
	if (PauseMenuClass)
	{
		PauseMenu = CreateWidget<UDkPauseMenuWidget>(GetOwningPlayerController(), PauseMenuClass);

		// Bind to the pause menu delegate
		if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayerController()))
		{
			if (FTogglePauseMenuSignature* PauseDelegate = PC->GetTogglePauseMenuDelegate())
			{
				PauseDelegate->AddUObject(this, &ADkHUD::HandleTogglePauseMenu);
			}
		}
	}
}

void ADkHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayerController()))
	{
		if (FTogglePauseMenuSignature* PauseDelegate = PC->GetTogglePauseMenuDelegate())
		{
			PauseDelegate->RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ADkHUD::HandleTogglePauseMenu()
{
	if (PauseMenu)
	{
		if (PauseMenu->IsInViewport())
		{
			PauseMenu->RemoveFromParent();
			if (APlayerController* PC = GetOwningPlayerController())
			{
				PC->SetPause(false);
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
			}
		}
		else
		{
			PauseMenu->AddToViewport(100);
			if (APlayerController* PC = GetOwningPlayerController())
			{
				PC->SetPause(true);
				PC->SetShowMouseCursor(true);
                
				FInputModeGameAndUI InputMode;
				// These settings should allow both UI and game input
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputMode.SetHideCursorDuringCapture(false);
				PC->SetInputMode(InputMode);
			}
		}
	}
}
// Copyright @ Christian Reichel

#include "Hud/DkUpgradeMenuWidget.h"
#include "Hud/DkHUD.h"
//#include "Components/ButtonComponent.h"

void UDkUpgradeMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddUObject(this, &UDkUpgradeMenuWidget::HandleResumeClicked);
	}
	
	if (UnlockHintButton)
	{
		UnlockHintButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonPressed);
		UnlockHintButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonReleased);
		UnlockHintButton->OnHovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonHovered);
		UnlockHintButton->OnUnhovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonUnhovered);
	}
        
	if (UnlockAbilityButton)
	{
		UnlockAbilityButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonPressed);
		UnlockAbilityButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonReleased);
		UnlockAbilityButton->OnHovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonHovered);
		UnlockAbilityButton->OnUnhovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonUnhovered);
	}
    
	if (UnlockUpgradeButton)
	{
		UnlockUpgradeButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonPressed);
		UnlockUpgradeButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonReleased);
		UnlockUpgradeButton->OnHovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonHovered);
		UnlockUpgradeButton->OnUnhovered().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonUnhovered);
	}
}

void UDkUpgradeMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// Track hold time for each button
	if (bIsHintButtonPressed)
	{
		HintButtonHoldTime += InDeltaTime;
		if (HintButtonHoldTime >= HoldDuration)
		{
			HandleUnlockHint();
			bIsHintButtonPressed = false;
			HintButtonHoldTime = 0.0f;
		}
	}
	
	if (bIsAbilityButtonPressed)
	{
		AbilityButtonHoldTime += InDeltaTime;
		if (AbilityButtonHoldTime >= HoldDuration)
		{
			HandleUnlockAbility();
			bIsAbilityButtonPressed = false;
			AbilityButtonHoldTime = 0.0f;
		}
	}
	
	if (bIsUpgradeButtonPressed)
	{
		UpgradeButtonHoldTime += InDeltaTime;
		if (UpgradeButtonHoldTime >= HoldDuration)
		{
			HandleUnlockUpgrade();
			bIsUpgradeButtonPressed = false;
			UpgradeButtonHoldTime = 0.0f;
		}
	}
}

void UDkUpgradeMenuWidget::HandleResumeClicked()
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (FToggleUpgradeMenuSignature* UpgradeDelegate = PC->GetToggleUpgradeMenuDelegate())
		{
			UpgradeDelegate->Broadcast();
		}
	}
}

void UDkUpgradeMenuWidget::HandleUnlockHintButtonPressed()
{
	bIsHintButtonPressed = true;
	HintButtonHoldTime = 0.0f;
}

void UDkUpgradeMenuWidget::HandleUnlockHintButtonReleased()
{
	bIsHintButtonPressed = false;
	HintButtonHoldTime = 0.0f;
}

void UDkUpgradeMenuWidget::HandleUnlockAbilityButtonPressed()
{
	bIsAbilityButtonPressed = true;
	AbilityButtonHoldTime = 0.0f;
}

void UDkUpgradeMenuWidget::HandleUnlockAbilityButtonReleased()
{
	bIsAbilityButtonPressed = false;
	AbilityButtonHoldTime = 0.0f;
}

void UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonPressed()
{
	bIsUpgradeButtonPressed = true;
	UpgradeButtonHoldTime = 0.0f;
}

void UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonReleased()
{
	bIsUpgradeButtonPressed = false;
	UpgradeButtonHoldTime = 0.0f;
}

// Hover event handlers implementation
void UDkUpgradeMenuWidget::HandleUnlockHintButtonHovered()
{
	HandleHintButtonHovered();
}

void UDkUpgradeMenuWidget::HandleUnlockHintButtonUnhovered()
{
	HandleHintButtonUnhovered();
}

void UDkUpgradeMenuWidget::HandleUnlockAbilityButtonHovered()
{
	HandleAbilityButtonHovered();
}

void UDkUpgradeMenuWidget::HandleUnlockAbilityButtonUnhovered()
{
	HandleAbilityButtonUnhovered();
}

void UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonHovered()
{
	HandleUpgradeButtonHovered();
}

void UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonUnhovered()
{
	HandleUpgradeButtonUnhovered();
}
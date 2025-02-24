// Copyright @ Christian Reichel

#include "Hud/DkUpgradeMenuWidget.h"
#include "Hud/DkHUD.h"
//#include "Components/ButtonComponent.h"

void UDkUpgradeMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (UnlockHintButton)
	{
		UnlockHintButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonPressed);
		UnlockHintButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockHintButtonReleased);
	}
        
	if (UnlockAbilityButton)
	{
		UnlockAbilityButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonPressed);
		UnlockAbilityButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockAbilityButtonReleased);
	}
    
	if (UnlockUpgradeButton)
	{
		UnlockUpgradeButton->OnPressed().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonPressed);
		UnlockUpgradeButton->OnReleased().AddUObject(this, &UDkUpgradeMenuWidget::HandleUnlockUpgradeButtonReleased);
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


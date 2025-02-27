// Copyright @ Christian Reichel

#include "Hud/DkUpgradeMenuWidget.h"

#include "Character/DkCharacter.h"
#include "Component/DkScanningComponent.h"
#include "Components/ProgressBar.h"
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

// In DkUpgradeMenuWidget.cpp, update the NativeTick function

void UDkUpgradeMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
    
	// Update button states each tick
	UpdateButtonStates();

	// Update progress bars
	if (HintProgressBar)
	{
		HintProgressBar->SetPercent(bIsHintButtonPressed ? 
			FMath::Min(HintButtonHoldTime / HoldDuration, 1.0f) : 0.0f);
	}
    
	if (AbilityProgressBar)
	{
		AbilityProgressBar->SetPercent(bIsAbilityButtonPressed ? 
			FMath::Min(AbilityButtonHoldTime / HoldDuration, 1.0f) : 0.0f);
	}
    
	if (UpgradeProgressBar)
	{
		UpgradeProgressBar->SetPercent(bIsUpgradeButtonPressed ? 
			FMath::Min(UpgradeButtonHoldTime / HoldDuration, 1.0f) : 0.0f);
	}
    
    
	// Track hold time for each button
	if (bIsHintButtonPressed)
	{
		HintButtonHoldTime += InDeltaTime;
		if (HintButtonHoldTime >= HoldDuration)
		{
			if (ConsumeResources(HintCost))
			{
				HandleUnlockHint();
			}
			bIsHintButtonPressed = false;
			HintButtonHoldTime = 0.0f;
		}
	}
    
	if (bIsAbilityButtonPressed)
	{
		AbilityButtonHoldTime += InDeltaTime;
		if (AbilityButtonHoldTime >= HoldDuration)
		{
			if (ConsumeResources(AbilityCost))
			{
				HandleUnlockAbility();
			}
			bIsAbilityButtonPressed = false;
			AbilityButtonHoldTime = 0.0f;
		}
	}
    
	if (bIsUpgradeButtonPressed)
	{
		UpgradeButtonHoldTime += InDeltaTime;
		if (UpgradeButtonHoldTime >= HoldDuration)
		{
			if (ConsumeResources(UpgradeCost))
			{
				HandleUnlockUpgrade();
			}
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

UDkScanningComponent* UDkUpgradeMenuWidget::GetScanningComponent()
{
	if (ADkPlayerController* PC = Cast<ADkPlayerController>(GetOwningPlayer()))
	{
		if (ADkCharacter* Character = Cast<ADkCharacter>(PC->GetPawn()))
		{
			return Character->FindComponentByClass<UDkScanningComponent>();
		}
	}
	return nullptr;
}

bool UDkUpgradeMenuWidget::HasEnoughResources(const TMap<EDkScanType, float>& Cost)
{
	UDkScanningComponent* ScanComponent = GetScanningComponent();
	if (!ScanComponent) return false;
    
	for (const auto& Pair : Cost)
	{
		if (!ScanComponent->HasResource(Pair.Key, Pair.Value))
		{
			return false;
		}
	}
	return true;
}

bool UDkUpgradeMenuWidget::ConsumeResources(const TMap<EDkScanType, float>& Cost)
{
	UDkScanningComponent* ScanComponent = GetScanningComponent();
	if (!ScanComponent) return false;
    
	// First check if we have enough resources
	if (!HasEnoughResources(Cost)) return false;
    
	// Then consume them
	for (const auto& Pair : Cost)
	{
		ScanComponent->ConsumeResource(Pair.Key, Pair.Value);
	}
    
	return true;
}

void UDkUpgradeMenuWidget::UpdateButtonStates()
{
	if (UnlockHintButton)
	{
		bool bHasEnoughForHint = HasEnoughResources(HintCost);
		UnlockHintButton->SetIsEnabled(bHasEnoughForHint);
	}
    
	if (UnlockAbilityButton)
	{
		bool bHasEnoughForAbility = HasEnoughResources(AbilityCost);
		UnlockAbilityButton->SetIsEnabled(bHasEnoughForAbility);
	}
    
	if (UnlockUpgradeButton)
	{
		bool bHasEnoughForUpgrade = HasEnoughResources(UpgradeCost);
		UnlockUpgradeButton->SetIsEnabled(bHasEnoughForUpgrade);
	}
}
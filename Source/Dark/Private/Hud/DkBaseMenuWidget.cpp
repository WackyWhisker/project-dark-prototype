// Copyright @ Christian Reichel

#include "Hud/DkBaseMenuWidget.h"

void UDkBaseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDkBaseMenuWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
    
	// Common setup for all menus when activated
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void UDkBaseMenuWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
    
	// Reset input when menu closes
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}
}
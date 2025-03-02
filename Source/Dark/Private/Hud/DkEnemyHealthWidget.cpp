// Copyright @ Christian Reichel

#include "Hud/DkEnemyHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"

void UDkEnemyHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	if (HealthBar)
	{
		HealthBar->SetPercent(1.0f);
	}
    
	if (BindingBar)
	{
		BindingBar->SetPercent(0.0f);
        
		// Initialize with no offset since health is full
		FWidgetTransform InitialTransform;
		InitialTransform.Translation = FVector2D(0.0f, 0.0f);
		BindingBar->SetRenderTransform(InitialTransform);
	}
}

void UDkEnemyHealthWidget::UpdateBars(float CurrentHealth, float MaxHealth, float CurrentBinding, float MaxBinding)
{
	if (!HealthBar || !BindingBar) return;

	float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
	float BindingPercent = FMath::Clamp(CurrentBinding / MaxBinding, 0.0f, 1.0f);
    
	// Update the bars
	HealthBar->SetPercent(HealthPercent);
	BindingBar->SetPercent(BindingPercent);

	// Get the actual width from the widget's geometry
	FVector2D BarSize = BindingBar->GetCachedGeometry().GetLocalSize();
	float HealthOffset = BarSize.X * (1.0f - HealthPercent);
    
	// Create and apply transform
	FWidgetTransform BindingTransform;
	BindingTransform.Translation = FVector2D(-HealthOffset, 0.0f);
	BindingBar->SetRenderTransform(BindingTransform);
}
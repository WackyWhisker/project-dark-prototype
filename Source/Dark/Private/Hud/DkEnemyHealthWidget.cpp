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
	}
}

void UDkEnemyHealthWidget::UpdateBars(float CurrentHealth, float MaxHealth, float CurrentBinding, float MaxBinding)
{
	if (!HealthBar || !BindingBar) return;

	float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
	float BindingPercent = FMath::Clamp(CurrentBinding / MaxBinding, 0.0f, 1.0f);
	BindingPercent = FMath::Min(BindingPercent, HealthPercent);
    
	// Update the bars
	HealthBar->SetPercent(HealthPercent);
	BindingBar->SetPercent(BindingPercent);

	// Check if we have valid geometry now
	if (BindingBar->GetCachedGeometry().GetLocalSize().X > 0)
	{
		bHasValidGeometry = true;
	}

	// If first binding damage and no valid geometry yet, queue an update
	if (!bHasValidGeometry && BindingPercent > 0)
	{
		FTimerHandle DummyHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DummyHandle,
			[this, HealthPercent]()
			{
				FVector2D BarSize = BindingBar->GetCachedGeometry().GetLocalSize();
				float HealthOffset = BarSize.X * (1.0f - HealthPercent);
                
				FWidgetTransform BindingTransform;
				BindingTransform.Translation = FVector2D(-HealthOffset, 0.0f);
				BindingBar->SetRenderTransform(BindingTransform);
			},
			0.05f, 
			false);
		return;
	}

	// Normal update with valid geometry
	FVector2D BarSize = BindingBar->GetCachedGeometry().GetLocalSize();
	float HealthOffset = BarSize.X * (1.0f - HealthPercent);
    
	FWidgetTransform BindingTransform;
	BindingTransform.Translation = FVector2D(-HealthOffset, 0.0f);
	BindingBar->SetRenderTransform(BindingTransform);
}
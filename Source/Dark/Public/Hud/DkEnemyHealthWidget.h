// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DkEnemyHealthWidget.generated.h"

class UProgressBar;
class UHorizontalBox;

UCLASS()
class DARK_API UDkEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BindingBar;

	virtual void NativeConstruct() override;
	void UpdateBars(float CurrentHealth, float MaxHealth, float CurrentBinding, float MaxBinding);

private:
	bool bHasValidGeometry = false;
};
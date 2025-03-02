// Copyright @ Christian Reichel

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DkEnemyHealthWidget.generated.h"

UCLASS()
class DARK_API UDkEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
    
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateBars(float CurrentHealth, float MaxHealth, float CurrentBinding, float MaxBinding);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* BindingBar;
};
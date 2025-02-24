// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkBaseMenuWidget.h"
#include "CommonButtonBase.h"
#include "Player/DkPlayerController.h"
#include "DkUpgradeMenuWidget.generated.h"

UCLASS()
class DARK_API UDkUpgradeMenuWidget : public UDkBaseMenuWidget
{
	GENERATED_BODY()

public:
	
	// Button action handlers - exposed to Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleUnlockHint();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleUnlockAbility();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleUnlockUpgrade();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockHintButton;
        
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockAbilityButton;
    
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockUpgradeButton;

	// Hold duration in seconds required to activate buttons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Settings")
	float HoldDuration = 1.5f;

private:
	// Button press tracking
	bool bIsHintButtonPressed = false;
	bool bIsAbilityButtonPressed = false;
	bool bIsUpgradeButtonPressed = false;
	
	// Hold timers
	float HintButtonHoldTime = 0.0f;
	float AbilityButtonHoldTime = 0.0f;
	float UpgradeButtonHoldTime = 0.0f;

	// Button press handlers
	UFUNCTION()
	void HandleUnlockHintButtonPressed();
	
	UFUNCTION()
	void HandleUnlockHintButtonReleased();
	
	UFUNCTION()
	void HandleUnlockAbilityButtonPressed();
	
	UFUNCTION()
	void HandleUnlockAbilityButtonReleased();
	
	UFUNCTION()
	void HandleUnlockUpgradeButtonPressed();
	
	UFUNCTION()
	void HandleUnlockUpgradeButtonReleased();
	
	
};
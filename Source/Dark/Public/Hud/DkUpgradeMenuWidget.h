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
	
	// Hover event handlers - exposed to Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleHintButtonHovered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleHintButtonUnhovered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleAbilityButtonHovered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleAbilityButtonUnhovered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleUpgradeButtonHovered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Upgrade Actions")
	void HandleUpgradeButtonUnhovered();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockHintButton;
        
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockAbilityButton;
    
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* UnlockUpgradeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* ResumeButton;

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
	void HandleResumeClicked();
	
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
	
	// Button hover handlers
	UFUNCTION()
	void HandleUnlockHintButtonHovered();
	
	UFUNCTION()
	void HandleUnlockHintButtonUnhovered();
	
	UFUNCTION()
	void HandleUnlockAbilityButtonHovered();
	
	UFUNCTION()
	void HandleUnlockAbilityButtonUnhovered();
	
	UFUNCTION()
	void HandleUnlockUpgradeButtonHovered();
	
	UFUNCTION()
	void HandleUnlockUpgradeButtonUnhovered();
};
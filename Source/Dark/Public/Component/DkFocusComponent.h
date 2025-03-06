// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkFocusComponent.generated.h"

class IDkPlayerControllerInterface;

UENUM(BlueprintType)
enum class EDkFocusMode : uint8
{
	None,
	Firearm,
	Scanning
};

DECLARE_MULTICAST_DELEGATE_OneParam(FFocusStateChanged, bool /* bIsFocused */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FFocusModeChanged, EDkFocusMode /* NewMode */, EDkFocusMode /* OldMode */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkFocusComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UDkFocusComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
	// State getters
	UFUNCTION(BlueprintPure, Category = "Focus")
	bool IsFocused() const { return bIsFocused; }
    
	UFUNCTION(BlueprintPure, Category = "Focus")
	EDkFocusMode GetCurrentMode() const { return CurrentMode; }
    
	// Toggle/Hold mode setting
	UFUNCTION(BlueprintCallable, Category = "Focus")
	void SetFocusToggleMode(bool bNewToggleMode);
    
	UFUNCTION(BlueprintPure, Category = "Focus")
	bool IsUsingToggleMode() const { return bUseFocusToggle; }
    
	// Delegates
	FFocusStateChanged OnFocusStateChanged;
	FFocusModeChanged OnFocusModeChanged;

private:
	bool bIsFocused;
	bool bUseFocusToggle;
	EDkFocusMode CurrentMode;
    
	// Interface handling
	void HandleFocusStart();
	void HandleFocusEnd();
	void HandleFocusModeSwitch();
    
	IDkPlayerControllerInterface* PlayerControllerInterface;
};
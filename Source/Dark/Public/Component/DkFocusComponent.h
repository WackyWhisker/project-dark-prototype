// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkFocusComponent.generated.h"

class IDkPlayerControllerInterface;
class UCameraComponent;
class UUserWidget;
class USpringArmComponent;

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
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
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

    // Widget handling
    UFUNCTION(BlueprintCallable, Category = "Focus|UI")
    void UpdateTargetWidget(bool bHasTarget);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float CameraInterpolationSpeed = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Offsets")
    float ScanningCameraOffset = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Offsets")
    float FirearmCameraOffset = -25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
    float ScanningArmLength = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
    float FirearmArmLength = 150.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera|Zoom")
    float DefaultArmLength = 400.0f;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Scanning")
    TSubclassOf<UUserWidget> ScanningCrosshairClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Scanning")
    TSubclassOf<UUserWidget> ScanningTargetCrosshairClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Firearm")
    TSubclassOf<UUserWidget> FirearmCrosshairClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Firearm")
    TSubclassOf<UUserWidget> FirearmTargetCrosshairClass;

private:
    bool bIsFocused;
    bool bUseFocusToggle;
    EDkFocusMode CurrentMode;
    
    // Interface handling
    void HandleFocusStart();
    void HandleFocusEnd();
    void HandleFocusModeSwitch();
    
    IDkPlayerControllerInterface* PlayerControllerInterface;

    // Camera handling
    UPROPERTY()
    UCameraComponent* PlayerCamera;

    UPROPERTY()
    USpringArmComponent* CameraBoom;
    
    float CurrentCameraOffset;
    float CurrentArmLength;
    FVector OriginalCameraLocation;

    void UpdateCamera(float DeltaTime);
    float GetTargetCameraOffset() const;
    float GetTargetArmLength() const;

    // Widget handling
    UPROPERTY()
    UUserWidget* CurrentWidget;

    void ClearWidget();
};
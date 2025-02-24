// Copyright @ Christian Reichel
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScannableComponent.generated.h"

UENUM(BlueprintType)
enum class EDkScanType : uint8
{
    Generic     UMETA(DisplayName = "Generic"),
    Ability     UMETA(DisplayName = "Ability"),
    PowerUp     UMETA(DisplayName = "Power Up")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkScannableComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDkScannableComponent();

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanning")
    float ScanDuration = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanning")
    bool bResetOnAbort = true;

    // Vibration Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanning|Vibration")
    float VibrationFrequency = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanning|Vibration")
    float BaseAmplitude = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanning|Vibration")
    float MaxAdditionalAmplitude = 12.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning|Type")
    EDkScanType ScanType = EDkScanType::Generic;

    UPROPERTY(EditInstanceOnly, Category = "Scanning|Type")
    int32 ScanValue = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning|Type")
    int32 MaxValue = 100;
    
    UPROPERTY(EditDefaultsOnly, Category = "Scanning|Type")
    bool bRetainOnDeath = false;

    
    // Core scanning interface
    void OnScanModeEntered();
    void OnScanModeExited();
    void OnScanStart();
    void OnScanProgress(float Progress);
    void OnScanComplete();
    void OnScanAborted();

    void HighlightAsTarget();
    void UnhighlightAsTarget();

    float GetScanDuration() const { return ScanDuration; }
    float GetCurrentScanProgress() const { return CurrentScanProgress; }

    UFUNCTION(BlueprintPure, Category = "Scanning")
    EDkScanType GetScanType() const { return ScanType; }

    UFUNCTION(BlueprintPure, Category = "Scanning")
    int32 GetScanValue() const { return ScanValue; }

    UFUNCTION(BlueprintPure, Category = "Scanning")
    int32 GetMaxValue() const { return MaxValue; }
    
    UFUNCTION(BlueprintPure, Category = "Scanning")
    bool ShouldRetainOnDeath() const { return bRetainOnDeath; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    float CurrentScanProgress = 0.0f;

    UPROPERTY()
    bool bIsBeingScanned = false;

private:
    FVector OriginalLocation = FVector::ZeroVector;
    FVector OriginalScale = FVector::OneVector;
    
    UPROPERTY(EditAnywhere, Category = "Scanning|Highlight")
    float HighlightScaleMultiplier = 1.2f;
    
    UPROPERTY()
    UMaterialInterface* OriginalMaterial = nullptr;
    
    UPROPERTY(EditAnywhere, Category = "Scanning|Highlight")
    UMaterialInterface* HighlightMaterial = nullptr;
};
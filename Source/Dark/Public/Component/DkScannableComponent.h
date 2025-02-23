#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScannableComponent.generated.h"

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

    // Core scanning interface
    void OnScanModeEntered();
    void OnScanModeExited();
    void OnScanStart();
    void OnScanProgress(float Progress);
    void OnScanComplete();
    void OnScanAborted();

    float GetScanDuration() const { return ScanDuration; }
    float GetCurrentScanProgress() const { return CurrentScanProgress; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    float CurrentScanProgress = 0.0f;

    UPROPERTY()
    bool bIsBeingScanned = false;

public:
    void HighlightAsTarget();
    void UnhighlightAsTarget();

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
// Copyright @ Christian Reichel
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScanningComponent.generated.h"

enum class EDkGameState : uint8;
enum class EDkScanType : uint8;
enum class EDkFocusMode : uint8;
class ADkPlayerController;
class ADkCharacter;
class UDkScannableComponent;
class IDkPlayerControllerInterface;
class UDkFocusComponent;

USTRUCT(BlueprintType)
struct FScanTypeValue
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scanning")
    float CurrentValue = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scanning")
    float MaxValue = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scanning")
    bool RetainOnDeath = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnScanValueChanged, EDkScanType, ScanType, float, OldValue, float, NewValue, float, MaxValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkScanningComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UDkScanningComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShowDebugTraces = true;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning")
    float TraceRadius = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning") 
    float TraceRange = 1500.0f;

    UPROPERTY(BlueprintAssignable, Category = "Scanning")
    FOnScanValueChanged OnScanValueChanged;

    UFUNCTION(BlueprintCallable, Category = "Scanning")
    bool HasResource(EDkScanType ScanType, float Amount) const;
    
    UFUNCTION(BlueprintCallable, Category = "Scanning")
    bool ConsumeResource(EDkScanType ScanType, float Amount);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY()
    class UDkGameStateSubsystem* GameStateSubsystem;

    UPROPERTY()
    UDkFocusComponent* FocusComponent;

    UFUNCTION()
    void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);
    
    void ResetValuesOnDeath();
   
    void HandleScanExecuteStart();
    void HandleScanExecuteEnd();
    
    void HandleFocusChanged(bool bIsFocused);
    void HandleFocusModeChanged(EDkFocusMode NewMode, EDkFocusMode OldMode);

    UFUNCTION()
    void OnScanModeStart();
   
    UFUNCTION()
    void OnScanModeEnd();
    
    UPROPERTY()
    ADkCharacter* PlayerRef = nullptr;

    UPROPERTY()
    ADkPlayerController* PlayerControllerRef = nullptr;

    UPROPERTY()
    UDkScannableComponent* CurrentScannableTarget = nullptr;

    UPROPERTY()
    bool bIsExecutingScanning = false;

    UPROPERTY()
    bool bIsScanningMode = false;

    // Stores the current values for each scan type
    UPROPERTY()
    TMap<EDkScanType, FScanTypeValue> ScannedValues;
};
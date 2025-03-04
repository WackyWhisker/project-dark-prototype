﻿// Copyright @ Christian Reichel
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScanningComponent.generated.h"

enum class EDkGameState : uint8;
enum class EDkScanType : uint8;
class ADkPlayerController;
class ADkCharacter;
class UCameraComponent;
class UDkScannableComponent;
class IDkPlayerControllerInterface;

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

public:
    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShowDebugTraces = true;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning")
    float TraceRadius = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Scanning") 
    float TraceRange = 1500.0f;
      
    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float CameraScanOffset = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera") 
    float CameraInterpolationSpeed = 5.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CrosshairWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CrosshairWidgetTargetClass;

    //Delegate for broadcast
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

    UFUNCTION()
    void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);
    
    void ResetValuesOnDeath();
    
    UFUNCTION()
    void OnScanModeStart();
   
    UFUNCTION()
    void OnScanModeEnd();
   
    UFUNCTION()
    void OnScanExecuteStart();
   
    UFUNCTION()
    void OnScanExecuteEnd();
    
    UPROPERTY()
    ADkCharacter* PlayerRef = nullptr;

    UPROPERTY()
    ADkPlayerController* PlayerControllerRef = nullptr;

    UPROPERTY()
    UCameraComponent* PlayerCameraRef = nullptr;

    UPROPERTY()
    class USceneComponent* CameraBoomRef;

    UPROPERTY()
    UDkScannableComponent* CurrentScannableTarget = nullptr;

    UPROPERTY()
    bool bIsInScanMode = false;

    UPROPERTY()
    bool bIsExecutingScanning = false;

    // Stores the current values for each scan type
    UPROPERTY()
    TMap<EDkScanType, FScanTypeValue> ScannedValues;

    IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

    float CurrentCameraOffset = 0.0f;
    FVector OriginalCameraLocation;

    UPROPERTY()
    UUserWidget* CrosshairWidget = nullptr;

};
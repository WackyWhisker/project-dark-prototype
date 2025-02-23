// Copyright @ Christian Reichel
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScanningComponent.generated.h"

class ADkPlayerController;
class ADkCharacter;
class UCameraComponent;
class UDkScannableComponent;
class IDkPlayerControllerInterface;

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

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
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

    IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

    float CurrentCameraOffset = 0.0f;
    FVector OriginalCameraLocation;

    UPROPERTY()
    UUserWidget* CrosshairWidget = nullptr;
};
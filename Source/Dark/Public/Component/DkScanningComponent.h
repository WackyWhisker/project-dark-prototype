﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkScanningComponent.generated.h"

class ADkPlayerController;
class ADkCharacter;
class IDkPlayerControllerInterface;
class UCameraComponent;
class USpringArmComponent;
class UDkScannableComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkScanningComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UDkScanningComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                            FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scanning")
    float ScannableSearchRange = 1500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scanning")
    float MaxViewConeAngle = 30.0f;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(BlueprintReadOnly)
    ADkCharacter* PlayerRef = nullptr;

    UPROPERTY(BlueprintReadOnly)
    ADkPlayerController* PlayerControllerRef = nullptr;

    UPROPERTY(BlueprintReadOnly)
    UCameraComponent* PlayerCameraRef = nullptr;


private:
    UFUNCTION()
    void OnScanModeStart();
    
    UFUNCTION()
    void OnScanModeEnd();
    
    UFUNCTION()
    void OnScanExecuteStart();
    
    UFUNCTION()
    void OnScanExecuteEnd();

    UFUNCTION()
    bool FindBestScannable(UDkScannableComponent*& OutBestScannable, float SphereRadius, float MaxAngleInDegrees);
    
    UFUNCTION()
    bool GetScannableComponentsInRange(TArray<UDkScannableComponent*>& OutScannables, float SphereRadius);
    
    UFUNCTION()
    bool FilterScannablesByViewCone(const TArray<UDkScannableComponent*>& PotentialScannables, 
                                  TArray<UDkScannableComponent*>& OutValidScannables, 
                                  float MaxAngleInDegrees);


    void InitiateSweepForScannables();

    
    UPROPERTY()
    UDkScannableComponent* CurrentScannableTarget = nullptr;

    UPROPERTY()
    bool bIsInScanMode = false;

    UPROPERTY()
    bool bIsExecutingScanning = false;

    IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;
};
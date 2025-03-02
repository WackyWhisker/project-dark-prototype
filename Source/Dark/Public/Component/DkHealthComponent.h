// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, Health, float, HealthDelta, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBindingChanged, float, CurrentBinding, float, MaxBinding);

class UDkMeleeDamageType;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UDkHealthComponent();

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnHealthDepleted OnHealthDepleted;

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnBindingChanged OnBindingChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    float MaxHealth;

    UFUNCTION(BlueprintCallable, Category="Health")
    void TakeDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category="Health")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintPure, Category="Health")
    bool IsAlive() const;

    UFUNCTION(BlueprintPure, Category="Health")
    float GetMaxHealth() const { return MaxHealth; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void InitializeComponent() override;

    // Hit Flash Properties
    UPROPERTY(EditAnywhere, Category = "Visual Effects", meta = (ToolTip = "Parameter name in the material to control flash intensity"))
    FName FlashIntensityParam = "FlashIntensity";

    UPROPERTY(EditAnywhere, Category = "Visual Effects", meta = (ToolTip = "How long the hit flash effect lasts"))
    float HitFlashDuration = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Visual Effects", meta = (ToolTip = "Color of the hit flash effect"))
    FLinearColor FlashColor = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);  // Default to reddish

    UPROPERTY(EditAnywhere, Category = "Visual Effects", meta = (ToolTip = "Parameter name for flash color in material"))
    FName FlashColorParam = "FlashColor";

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY()
    UMaterialInterface* OriginalMaterial;

    // Damage Handler
    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
       const class UDamageType* DamageType, class AController* InstigatedBy,
       AActor* DamageCauser);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
    float DamageCooldownDuration = 0.5f;

    UFUNCTION()
    void ApplyDamagePushback(AActor* DamagedActor, float Damage, AActor* DamageCauser);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pushback")
    float DamagePushbackStrength = 1000.0f;

    // State change handler
    UFUNCTION()
    void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);
    
    // Track if we're registered for reset
    bool bIsRegisteredForReset = false;

private:
    bool bIsDead;
    float CurrentBinding = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Binding")
    float BindingThreshold = -1.0f; // -1 means no threshold
    
    FTimerHandle DamageCooldownTimer;
    FTimerHandle HitFlashTimerHandle;
    bool bCanTakeDamage = true;

    // Visual feedback methods
    void PlayHitFlash();
    void ResetDamageCooldown();
    
    // Damage processing methods
    void ProcessBindingDamage(float Damage);
    void ProcessMeleeDamage(float Damage, const class UDkMeleeDamageType* DamageType);
};
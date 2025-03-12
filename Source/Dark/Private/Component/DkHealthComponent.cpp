// Copyright @ Christian Reichel

#include "Component/DkHealthComponent.h"
#include "GameFramework/Character.h"
#include "Interface/DkDeathHandlerInterface.h"
#include "Subsystem/DkGameStateSubsystem.h"
#include "Combat/DkBindingDamageType.h"
#include "Combat/DkMeleeDamageType.h"

UDkHealthComponent::UDkHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    bIsDead = false;
}

void UDkHealthComponent::InitializeComponent()
{
    Super::InitializeComponent();
    CurrentHealth = MaxHealth; 
}

void UDkHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UDkHealthComponent::HandleTakeAnyDamage);
        
        if (ACharacter* Character = Cast<ACharacter>(Owner))
        {
            OwnerMesh = Character->GetMesh();
            if (OwnerMesh)
            {
                OriginalMaterial = OwnerMesh->GetMaterial(0);
            }
        }
    }
    
    if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
    {
        GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UDkHealthComponent::HandleGameStateChanged);
    }
}

void UDkHealthComponent::PlayHitFlash()
{
    if (!OwnerMesh || !OriginalMaterial) return;
   
    UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
    if (!DynamicMat) return;
   
    DynamicMat->SetScalarParameterValue(FlashIntensityParam, 1.0f);
    DynamicMat->SetVectorParameterValue(FlashColorParam, FlashColor);
    OwnerMesh->SetMaterial(0, DynamicMat);
   
    if (GetWorld()->GetTimerManager().IsTimerActive(HitFlashTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimerHandle);
    }
   
    GetWorld()->GetTimerManager().SetTimer(
        HitFlashTimerHandle,
        [this, DynamicMat]()
        {
            if (DynamicMat)
            {
                DynamicMat->SetScalarParameterValue(FlashIntensityParam, 0.0f);
            }
        },
        HitFlashDuration,
        false
    );
}

void UDkHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || bIsDead) return;

    // Handle binding damage
    if (const UDkBindingDamageType* BindingDamage = Cast<UDkBindingDamageType>(DamageType))
    {
        ProcessBindingDamage(Damage);
        return;
    }

    // Handle melee damage
    if (const UDkMeleeDamageType* MeleeDamage = Cast<UDkMeleeDamageType>(DamageType))
    {
        ProcessMeleeDamage(Damage, MeleeDamage);
        ApplyDamagePushback(DamagedActor, Damage, DamageCauser);
        return;
    }

    // Handle any other damage type normally
    TakeDamage(Damage);
    ApplyDamagePushback(DamagedActor, Damage, DamageCauser);
}

void UDkHealthComponent::ProcessBindingDamage(float Damage)
{
    float OldBinding = CurrentBinding;
    float MaxPossibleBinding = (BindingThreshold > 0.0f) ? BindingThreshold : MaxHealth;
    
    CurrentBinding = FMath::Clamp(CurrentBinding + Damage, 0.0f, MaxPossibleBinding);
    
    if (CurrentBinding != OldBinding)
    {
        OnBindingChanged.Broadcast(CurrentBinding, MaxPossibleBinding);
        UE_LOG(LogTemp, Display, TEXT("Binding Damage %f"), CurrentBinding);
        PlayHitFlash();
    }
}

void UDkHealthComponent::ProcessMeleeDamage(float Damage, const UDkMeleeDamageType* DamageType)
{
    // Take the larger value between base melee damage and current binding
    float TotalDamage = FMath::Max(Damage, CurrentBinding);
    
    // Reset binding after use
    CurrentBinding = 0.0f;
    OnBindingChanged.Broadcast(CurrentBinding, MaxHealth);
    
    // Apply the total damage
    UE_LOG(LogTemp, Display, TEXT("Total Damage %f"), TotalDamage);
    PlayHitFlash();
    TakeDamage(TotalDamage);
}

void UDkHealthComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f || bIsDead || !bCanTakeDamage) return;

    bCanTakeDamage = false;
    
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    UE_LOG(LogTemp, Display, TEXT("Health %f"), CurrentHealth);
    
    OnHealthChanged.Broadcast(CurrentHealth, -DamageAmount, GetMaxHealth());
    
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        OnHealthDepleted.Broadcast();

        if (AActor* Owner = GetOwner())
        {
            if (Owner->Implements<UDkDeathHandlerInterface>())
            {
                IDkDeathHandlerInterface::Execute_HandleDeath(Owner);
            }
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        DamageCooldownTimer,
        this,
        &UDkHealthComponent::ResetDamageCooldown,
        DamageCooldownDuration,
        false
    );
}

void UDkHealthComponent::Heal(float HealAmount)
{
    if (HealAmount <= 0.0f || bIsDead) return;

    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    float ActualHealAmount = CurrentHealth - OldHealth;

    if (ActualHealAmount > 0.0f)
    {
        OnHealthChanged.Broadcast(CurrentHealth, ActualHealAmount, GetMaxHealth());
    }
}

void UDkHealthComponent::ApplyDamagePushback(AActor* DamagedActor, float Damage, AActor* DamageCauser)
{
    if (!DamagedActor || !DamageCauser) return;

    ACharacter* OwningCharacter = Cast<ACharacter>(DamagedActor);
    if (!OwningCharacter) return;

    FVector PushDirection = (DamagedActor->GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
    
    // Zero out the vertical component to prevent lifting
    PushDirection.Z = 0.0f;
    
    // Renormalize to maintain original horizontal magnitude
    PushDirection = PushDirection.GetSafeNormal();
    
    OwningCharacter->LaunchCharacter(PushDirection * DamagePushbackStrength, false, false);
}

bool UDkHealthComponent::IsAlive() const
{
    return !bIsDead;
}

void UDkHealthComponent::HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
    if (NewState == EDkGameState::Respawning)
    {
        CurrentHealth = MaxHealth;
        CurrentBinding = 0.0f;
        bIsDead = false;
        bCanTakeDamage = true;
        
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth - CurrentHealth, GetMaxHealth());
        OnBindingChanged.Broadcast(CurrentBinding, MaxHealth);
    }
}

void UDkHealthComponent::ResetDamageCooldown()
{
    bCanTakeDamage = true;
}

void UDkHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
    {
        GameStateSubsystem->OnGameStateChanged.RemoveDynamic(this, &UDkHealthComponent::HandleGameStateChanged);
        
        if (bIsRegisteredForReset)
        {
            GameStateSubsystem->NotifyResetComplete(this);
            bIsRegisteredForReset = false;
        }
    }
}



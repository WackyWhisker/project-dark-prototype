// Copyright @ Christian Reichel

#include "Component/DkFirearmComponent.h"
#include "Component/DkFocusComponent.h"
#include "GameFramework/Character.h"
#include "Player/DkPlayerControllerInterface.h"

UDkFirearmComponent::UDkFirearmComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDkFirearmComponent::BeginPlay()
{
    Super::BeginPlay();
    bCanFire = true;
    
    // Get focus component reference
    FocusComponent = GetOwner()->FindComponentByClass<UDkFocusComponent>();
    if (FocusComponent)
    {
        FocusComponent->OnFocusStateChanged.AddUObject(this, &UDkFirearmComponent::HandleFocusChanged);
        FocusComponent->OnFocusModeChanged.AddUObject(this, &UDkFirearmComponent::HandleFocusModeChanged);
    }
    
    // Get player controller interface
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn)
    {
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            if (IDkPlayerControllerInterface* Interface = Cast<IDkPlayerControllerInterface>(PC))
            {
                // Subscribe to shoot delegate
                Interface->GetShootDelegate()->AddUObject(this, &UDkFirearmComponent::Fire);
            }
        }
    }
    
    // Spawn default weapon if set
    if (DefaultWeapon)
    {
        EquipWeapon(DefaultWeapon);
    }
}

void UDkFirearmComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (FocusComponent)
    {
        FocusComponent->OnFocusStateChanged.RemoveAll(this);
        FocusComponent->OnFocusModeChanged.RemoveAll(this);
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->OnAmmoStateChanged.RemoveAll(this);
    }
}

void UDkFirearmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDkFirearmComponent::HandleFocusChanged(bool bIsFocused)
{
    if (!CurrentWeapon) return;
    
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (bIsFocused && bIsFirearmMode)
    {
        CurrentWeapon->AttachToHand(Character->GetMesh());
    }
    else
    {
        CurrentWeapon->AttachToHolster(Character->GetMesh());
    }
}

void UDkFirearmComponent::HandleFocusModeChanged(EDkFocusMode NewMode, EDkFocusMode OldMode)
{
    bIsFirearmMode = (NewMode == EDkFocusMode::Firearm);
    
    // Update weapon position if focus is active
    if (FocusComponent && FocusComponent->IsFocused())
    {
        HandleFocusChanged(true);
    }
}

void UDkFirearmComponent::Fire()
{
    if (!bCanFire || !CurrentWeapon || !bIsFirearmMode) return;
    
    if (FocusComponent && FocusComponent->IsFocused() && CurrentWeapon->CanFire())
    {
        CurrentWeapon->Fire();
        OnWeaponFired.Broadcast();
        bCanFire = false;
        GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &UDkFirearmComponent::EndFireCooldown, FireRate, false);
    }
}

void UDkFirearmComponent::Reload()
{
    if (CurrentWeapon && bIsFirearmMode)
    {
        CurrentWeapon->StartReload();
    }
}

void UDkFirearmComponent::EndFireCooldown()
{
    bCanFire = true;
}

void UDkFirearmComponent::EquipWeapon(TSubclassOf<ADkFirearmBase> WeaponClass)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->OnAmmoStateChanged.RemoveAll(this);
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }
    
    if (!WeaponClass || !GetOwner()) return;
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    
    CurrentWeapon = GetWorld()->SpawnActor<ADkFirearmBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    if (CurrentWeapon)
    {
        ACharacter* Character = Cast<ACharacter>(GetOwner());
        if (Character)
        {
            CurrentWeapon->AttachToHolster(Character->GetMesh());
        }
        
        // Subscribe to weapon's ammo changes
        CurrentWeapon->OnAmmoStateChanged.AddDynamic(this, &UDkFirearmComponent::OnWeaponAmmoChanged);
        OnWeaponAmmoChanged(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetMaxAmmo());
    }
}

void UDkFirearmComponent::OnWeaponAmmoChanged(int32 InCurrentAmmo, int32 InMaxAmmo)
{
    if (CurrentWeapon)
    {
        OnAmmoChanged.Broadcast(InCurrentAmmo, InMaxAmmo, CurrentWeapon);
    }
}
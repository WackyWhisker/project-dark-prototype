// Copyright @ Christian Reichel

#include "Component/DkFirearmComponent.h"
#include "GameFramework/Character.h"
#include "Player/DkPlayerControllerInterface.h"

UDkFirearmComponent::UDkFirearmComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDkFirearmComponent::BeginPlay()
{
    Super::BeginPlay();
    bIsAiming = false;
    bCanFire = true;
    
    // Get player controller interface
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn)
    {
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            if (IDkPlayerControllerInterface* Interface = Cast<IDkPlayerControllerInterface>(PC))
            {
                // Subscribe to aiming delegates
                Interface->GetAimStartDelegate()->AddUObject(this, &UDkFirearmComponent::StartAiming);
                Interface->GetAimEndDelegate()->AddUObject(this, &UDkFirearmComponent::StopAiming);
                
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

void UDkFirearmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDkFirearmComponent::StartAiming()
{
    bIsAiming = true;
    
    if (CurrentWeapon && GetOwner())
    {
        ACharacter* Character = Cast<ACharacter>(GetOwner());
        if (Character)
        {
            CurrentWeapon->AttachToHand(Character->GetMesh());
        }
    }
}

void UDkFirearmComponent::StopAiming()
{
    bIsAiming = false;
    
    if (CurrentWeapon && GetOwner())
    {
        ACharacter* Character = Cast<ACharacter>(GetOwner());
        if (Character)
        {
            CurrentWeapon->AttachToHolster(Character->GetMesh());
        }
    }
}

void UDkFirearmComponent::Fire()
{
    if (!bCanFire || !CurrentWeapon) return;
    
    if (bIsAiming && CurrentWeapon->CanFire())
    {
        CurrentWeapon->Fire();
        OnWeaponFired.Broadcast();
        bCanFire = false;
        GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &UDkFirearmComponent::EndFireCooldown, FireRate, false);
    }
}

void UDkFirearmComponent::Reload()
{
    if (CurrentWeapon)
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
    // Destroy current weapon if exists
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }
    
    if (!WeaponClass || !GetOwner()) return;
    
    // Spawn new weapon
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    
    CurrentWeapon = GetWorld()->SpawnActor<ADkFirearmBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    if (CurrentWeapon)
    {
        // Attach to holster initially
        ACharacter* Character = Cast<ACharacter>(GetOwner());
        if (Character)
        {
            CurrentWeapon->AttachToHolster(Character->GetMesh());
        }
    }
}
// Copyright @ Christian Reichel

#include "Weapon/DkPistol.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

ADkPistol::ADkPistol()
{
    FireRate = 0.2f;
    Damage = 25.0f;
    MaxAmmo = 12;
}

void ADkPistol::BeginPlay()
{
    Super::BeginPlay();
}

void ADkPistol::Fire()
{
    if (!Super::CanFire()) return;
    
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;
    
    // Get camera view point
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;
    
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
    
    // Primary trace from camera
    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + CameraRotation.Vector() * Range;
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());
    
    FHitResult CameraHit;
    bool bHit = GetWorld()->LineTraceSingleByChannel(CameraHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
    
    DrawDebugLine(GetWorld(), TraceStart, bHit ? CameraHit.Location : TraceEnd, FColor::Green, false, 2.0f);
    
    // If we hit something, do muzzle trace for effects
    if (bHit)
    {
        FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
        
        FHitResult MuzzleHit;
        GetWorld()->LineTraceSingleByChannel(MuzzleHit, MuzzleLocation, CameraHit.Location, ECC_Visibility, QueryParams);
        
        DrawDebugLine(GetWorld(), MuzzleLocation, CameraHit.Location, FColor::Red, false, 2.0f);
        
        ProcessHit(CameraHit, MuzzleHit);
    }
    
    Super::Fire(); // Handle ammo and reload
}

void ADkPistol::ProcessHit(const FHitResult& CameraHit, const FHitResult& MuzzleHit)
{
    // Camera hit is for gameplay (binding)
    DrawDebugPoint(GetWorld(), CameraHit.ImpactPoint, 10.0f, FColor::Green, false, 1.0f);
    
    // Apply binding damage
    if (AActor* HitActor = CameraHit.GetActor())
    {
        FDamageEvent DamageEvent(UDkBindingDamageType::StaticClass());
        HitActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
    }
    
    // Muzzle hit is for effects (particles, decals)
    if (MuzzleHit.bBlockingHit)
    {
        DrawDebugPoint(GetWorld(), MuzzleHit.ImpactPoint, 10.0f, FColor::Red, false, 1.0f);
    }
}
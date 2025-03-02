// Copyright @ Christian Reichel

#include "Weapon/DkPistol.h"
#include "DrawDebugHelpers.h"

ADkPistol::ADkPistol()
{
	// Set default values
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
    
	// Get pistol muzzle location
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
	FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(FName("Muzzle"));
    
	// Setup trace
	FVector Start = MuzzleLocation;
	FVector End = Start + MuzzleRotation.Vector() * Range;
    
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
    
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
    
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
    
	if (bHit)
	{
		ProcessHit(Hit);
	}
    
	Super::Fire(); // Handle ammo and reload
}

void ADkPistol::ProcessHit(const FHitResult& Hit)
{
	// For now just draw debug point
	DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 20.0f, FColor::Red, false, 2.0f);
    
	// Later add damage, effects, etc.
}
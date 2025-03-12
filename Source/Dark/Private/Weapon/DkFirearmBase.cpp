// Copyright @ Christian Reichel

#include "Weapon/DkFirearmBase.h"

const FName ADkFirearmBase::HolsterSocket = FName("Holster_Socket");
const FName ADkFirearmBase::HandSocket = FName("Hand_R");

ADkFirearmBase::ADkFirearmBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void ADkFirearmBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
	OnAmmoStateChanged.Broadcast(CurrentAmmo, MaxAmmo);
}

void ADkFirearmBase::Fire()
{
	if (!CanFire()) return;
    
	CurrentAmmo--;
	OnAmmoStateChanged.Broadcast(CurrentAmmo, MaxAmmo);
    
	if (CurrentAmmo <= 0 && bAutoReload)
	{
		StartReload();
	}
}

void ADkFirearmBase::StartReload()
{
	if (bIsReloading || CurrentAmmo == MaxAmmo) return;
    
	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ADkFirearmBase::FinishReload, ReloadTime, false);
}

void ADkFirearmBase::FinishReload()
{
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
	OnAmmoStateChanged.Broadcast(CurrentAmmo, MaxAmmo);
}

bool ADkFirearmBase::CanFire() const
{
	return !bIsReloading && CurrentAmmo > 0;
}

bool ADkFirearmBase::IsReloading() const
{
	return bIsReloading;
}

void ADkFirearmBase::AttachToHolster(USkeletalMeshComponent* CharacterMesh)
{
	if (!CharacterMesh) return;
	WeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HolsterSocket);
}

void ADkFirearmBase::AttachToHand(USkeletalMeshComponent* CharacterMesh)
{
	if (!CharacterMesh) return;
	WeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocket);
}
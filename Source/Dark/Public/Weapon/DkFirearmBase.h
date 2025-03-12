// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Combat/DkBindingDamageType.h"
#include "Engine/DamageEvents.h"
#include "DkFirearmBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoStateChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS()
class DARK_API ADkFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	ADkFirearmBase();

	virtual void Fire();
	virtual void StartReload();
    
	void AttachToHolster(USkeletalMeshComponent* CharacterMesh);
	void AttachToHand(USkeletalMeshComponent* CharacterMesh);
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanFire() const;
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsReloading() const;
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnAmmoStateChanged OnAmmoStateChanged;

protected:
	virtual void BeginPlay() override;

	static const FName HolsterSocket;
	static const FName HandSocket;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 20.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate = 0.25f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadTime = 1.5f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmo = 12;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bAutoReload = true;

private:
	bool bIsReloading;
	int32 CurrentAmmo;
    
	FTimerHandle ReloadTimerHandle;
    
	void FinishReload();
};
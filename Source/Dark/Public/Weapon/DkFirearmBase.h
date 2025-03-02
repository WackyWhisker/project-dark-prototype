// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DkFirearmBase.generated.h"

UCLASS()
class DARK_API ADkFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	ADkFirearmBase();

	// Basic weapon functions
	virtual void Fire();
	virtual void StartReload();
    
	// Attachment functions
	void AttachToHolster(USkeletalMeshComponent* CharacterMesh);
	void AttachToHand(USkeletalMeshComponent* CharacterMesh);
    
	// Getters
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanFire() const;
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsReloading() const;
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMaxAmmo() const { return MaxAmmo; }

protected:
	virtual void BeginPlay() override;

	// Socket names
	static const FName HolsterSocket;
	static const FName HandSocket;

	// Components
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;
    
	// Weapon properties
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
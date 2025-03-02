// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/DkFirearmBase.h"
#include "DkFirearmComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFired);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkFirearmComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UDkFirearmComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Firearm functions
	void StartAiming();
	void StopAiming();
	void Fire();
	void Reload();

	UPROPERTY(BlueprintAssignable, Category = "Firearms")
	FOnWeaponFired OnWeaponFired;
    
	// Weapon management
	UFUNCTION(BlueprintCallable, Category = "Firearms")
	void EquipWeapon(TSubclassOf<ADkFirearmBase> WeaponClass);
    
	UFUNCTION(BlueprintPure, Category = "Firearms")
	ADkFirearmBase* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	bool bIsAiming;
    
	UPROPERTY(EditDefaultsOnly, Category = "Firearms")
	float FireRate = 0.25f;
    
	FTimerHandle FireRateTimerHandle;
	bool bCanFire = true;

	void EndFireCooldown();
    
private:
	UPROPERTY()
	ADkFirearmBase* CurrentWeapon;
    
	UPROPERTY(EditDefaultsOnly, Category = "Firearms")
	TSubclassOf<ADkFirearmBase> DefaultWeapon;
};
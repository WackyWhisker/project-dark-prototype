// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, HealthDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);

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

protected:
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
		const class UDamageType* DamageType, class AController* InstigatedBy,
		AActor* DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
	float DamageCooldownDuration = 0.5f;

	UFUNCTION()
	void ApplyDamagePushback(AActor* DamagedActor, float Damage, AActor* DamageCauser);
//TODO: PUSH BACK IN ITS OWN COMP
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pushback")
	float DamagePushbackStrength = 1000.0f;

private:
	bool bIsDead;

	FTimerHandle DamageCooldownTimer;
	bool bCanTakeDamage = true;

	void ResetDamageCooldown();

	
};

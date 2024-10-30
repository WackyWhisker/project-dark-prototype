// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, HealthDelta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDkHealthComponent();

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChanged OnHealthChanged;

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

private:
	bool bIsDead;

	
};

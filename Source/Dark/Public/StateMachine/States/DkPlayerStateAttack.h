// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateAttack.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateAttack : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	// public methods
	virtual void TickState() override;

public:
	// public properties

protected:
	// protected methods
	virtual void Attack() override;
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

protected:
	// protected properties
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage;

	int32 AttackIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	float AttackLaunchStrength = 1.0f;

private:
	// private methods
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void AttackLaunchCharacter();

private:
	// private properties
	bool IsAttacking = false;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	float PreviousBrakingFrictionFactor;
	float PreviousBrakingDecelerationWalking;
};
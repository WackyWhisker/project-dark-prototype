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
	virtual void TickState() override;

protected:
	virtual void Attack() override;
	virtual void OnStateEnter(AActor* StateOwner) override;
	

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage; //TODO: Combat system likely needs an array or TMAP of anims

	int32 AttackIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	float AttackLaunchStrength = 1.0f;

private:
	bool IsAttacking = false;
	UAnimInstance* AnimInstance;
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackLaunchCharacter();

	//Movement related
	float PreviousBrakingFrictionFactor;
	float PreviousBrakingDecelerationWalking;
	
};

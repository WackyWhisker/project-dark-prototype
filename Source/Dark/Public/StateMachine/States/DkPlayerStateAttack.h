// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateAttack.generated.h"

UCLASS()
class DARK_API UDkPlayerStateAttack : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	virtual void Attack() override;
    
	void OpenAttackWindow();
	void CloseAttackWindow();

protected:
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly)
	FName FirstAttackSection = "Attack01";

	UPROPERTY(EditDefaultsOnly)
	FName SecondAttackSection = "Attack02";

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool IsAttacking = false;
	bool IsInAttackWindow = false;

	UPROPERTY()
	UAnimInstance* AnimInstance;
};
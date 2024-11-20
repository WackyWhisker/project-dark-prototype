// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateDodge.generated.h"

UCLASS()
class DARK_API UDkPlayerStateDodge : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;

protected:
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DodgeLeftMontage; 

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DodgeRightMontage;

	UPROPERTY(EditDefaultsOnly)
	float DodgeLaunchStrength = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float DodgeZStrength = 300.0f;

private:
	UPROPERTY()
	UAnimInstance* AnimInstance = nullptr;

	void DodgeLaunchCharacter();

	bool bHasLaunched = false;

	
};

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
	// Public methods
	virtual void TickState() override;

public:
	// Public properties

protected:
	// Protected methods
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

protected:
	// Protected properties
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DodgeLeftMontage; 

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DodgeRightMontage;

	UPROPERTY(EditDefaultsOnly)
	float DodgeLaunchStrength = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float DodgeZStrength = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge", meta = (ToolTip = "Minimum time in seconds before state can transition"))
	float MinStateTransitionTime = 1.0f;

private:
	// Private methods
	void DodgeLaunchCharacter();
	void EnableStateTransition();

private:
	// Private properties
	UPROPERTY()
	UAnimInstance* AnimInstance = nullptr;

	bool bCanTransition = false;
	FTimerHandle DodgeTimerHandle;

	bool bHasLaunched = false;
};
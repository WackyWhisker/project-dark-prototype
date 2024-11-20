// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkStateBase.h"
#include "Character/DkCharacter.h"
#include "Player/DkPlayerController.h"
#include "DkPlayerStateBase.generated.h"

class IDkPlayerControllerInterface;

UCLASS()
class DARK_API UDkPlayerStateBase : public UDkStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ADkCharacter* PlayerRef = nullptr;

	UPROPERTY(BlueprintReadOnly)
	ADkPlayerController* PlayerControllerRef = nullptr;

	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

protected:
	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

	//Matching functions for the player controller delegates
	virtual void Jump();
	virtual void Attack();
	virtual void Dodge();
};

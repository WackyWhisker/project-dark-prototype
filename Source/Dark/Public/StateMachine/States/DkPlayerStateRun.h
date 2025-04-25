// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkPlayerStateBase.h"
#include "DkPlayerStateRun.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateRun : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	
protected:
	virtual void Attack() override;
	virtual void Dodge() override;
	virtual void OnStateEnter(AActor* StateOwner) override;
};

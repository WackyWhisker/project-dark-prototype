// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkPlayerStateBase.h"
#include "DkPlayerStateAir.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateAir : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	
protected:
	virtual void Jump() override;
	virtual void OnStateEnter(AActor* StateOwner) override;
	
};

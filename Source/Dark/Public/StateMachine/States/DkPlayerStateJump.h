// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkPlayerStateAir.h"
#include "DkPlayerStateJump.generated.h"


UCLASS()
class DARK_API UDkPlayerStateJump : public UDkPlayerStateAir
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	
protected:
	virtual void OnStateEnter(AActor* StateOwner) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AirYaw = 150.0f;
};
//TODO: Handle double jump in this state
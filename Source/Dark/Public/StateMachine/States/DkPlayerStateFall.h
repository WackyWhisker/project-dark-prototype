﻿// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateFall.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateFall : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	
protected:
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NearGroundDistance = 75.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FallGravity = 2.0f;

private:
	bool IsNearGround();
};

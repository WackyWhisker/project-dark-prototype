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

private:
	bool IsNearGround();
};

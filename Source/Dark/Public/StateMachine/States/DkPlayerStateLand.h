// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateLand.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateLand : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;
	
protected:
	virtual void OnStateEnter(AActor* StateOwner) override;
};

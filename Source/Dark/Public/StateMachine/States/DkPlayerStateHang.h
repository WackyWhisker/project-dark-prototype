// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerStateBase.h"
#include "DkPlayerStateAir.h"
#include "DkPlayerStateHang.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API UDkPlayerStateHang : public UDkPlayerStateBase
{
	GENERATED_BODY()

protected:

	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void TickState() override;
	virtual void OnStateExit() override;

private:
	
	FLedgeDetectionData LedgeData;
	
};

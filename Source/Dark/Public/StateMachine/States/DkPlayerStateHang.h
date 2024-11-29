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

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Hanging")
	float SnapSpeed = 20.0f;

private:
	
	FLedgeDetectionData LedgeData;

	// Snap interpolation
	FVector SnapStartPosition;
	FVector SnapTargetPosition;
	FRotator SnapStartRotation;
	FRotator SnapTargetRotation;
   
	float SnapAlpha = 0.0f;
	bool bIsSnapping = false;  // Also renamed this to match
   
	
	
};

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

public:
	// public methods

	// public properties

protected:
	// protected methods
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void TickState() override;
	virtual void OnStateExit() override;
	virtual void Drop() override;
	virtual void Lift() override;

	// protected properties
	UPROPERTY(EditDefaultsOnly, Category = "Hanging")
	float SnapSpeed = 20.0f;

private:
	// private methods

	// private properties
	FLedgeDetectionData LedgeData;

	// Snap interpolation
	FVector SnapStartPosition;
	FVector SnapTargetPosition;
	FRotator SnapStartRotation;
	FRotator SnapTargetRotation;
   
	float SnapAlpha = 0.0f;
	bool bIsSnapping = false;

	// Climbing
	FVector ClimbStartPosition;
	FVector ClimbTargetPosition;
	float ClimbAlpha = 0.0f;
	bool bIsClimbing = false;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing")
	float ClimbSpeed = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Climbing")
	float ClimbHeightCurve = 100.0f;  // Max height of the arc

	UPROPERTY(EditDefaultsOnly, Category = "Climbing")
	float ClimbForwardCurve = 50.0f;  // How far forward during arc
	
};
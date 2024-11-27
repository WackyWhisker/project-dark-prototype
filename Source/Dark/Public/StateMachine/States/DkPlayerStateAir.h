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
	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;

	//Ledge climbing related
	UPROPERTY(EditDefaultsOnly)
	float TraceStartHeightOffset = 150.0f; //Aprox chest height
	
	UPROPERTY(EditDefaultsOnly)
	float TraceForWallDistance = 100.0f; //Aprox chest height

	UPROPERTY(EditDefaultsOnly)
	float LedgeCheckUpwardsOffset = 50.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float LedgeCheckInwardsOffset = 25.0f;

	UPROPERTY(EditDefaultsOnly)
	float LedgeCheckSideOffset = 45.0f;

	UPROPERTY(EditDefaultsOnly)
	float LedgeCheckDownwardTraceLength = 200.0f;
	
	UFUNCTION()
	bool CheckForWallInFront(FHitResult& OutHit);

	UFUNCTION()
	bool CheckForLedgeAbove(const FHitResult& InWallHit, FHitResult& OutLedgeHit);

private:
	//Ledge climbing related
	FHitResult WallHit;
	FHitResult LedgeHit;
};

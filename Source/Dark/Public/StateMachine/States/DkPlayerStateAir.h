// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkPlayerStateBase.h"
#include "DkPlayerStateAir.generated.h"

USTRUCT()
struct FLedgeDetectionData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsValid = false;

	UPROPERTY()
	FVector CenterHitLocation;

	UPROPERTY()
	FVector WallNormal;

	UPROPERTY()
	FVector WallHitLocation;

	UPROPERTY()
	FVector IdealHangPosition;

	void Reset()
	{
		bIsValid = false;
		CenterHitLocation = FVector::ZeroVector;
		WallHitLocation = FVector::ZeroVector;
		WallNormal = FVector::ZeroVector;
		IdealHangPosition = FVector::ZeroVector;
	}
};

UCLASS()
class DARK_API UDkPlayerStateAir : public UDkPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void TickState() override;

	UFUNCTION()
	const FLedgeDetectionData& GetLedgeData() const { return LedgeData; }

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShowDebugVisuals = true;
	
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
	
	UPROPERTY(EditDefaultsOnly)
	float HangPositionHeightOffset = -100.0f;

	UPROPERTY(EditDefaultsOnly)
	float HangPositionWallOffset = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	float HangPositionTolerance = 100.0f;
	
	UFUNCTION()
	bool CheckForWallInFront(FHitResult& OutHit);
	
	// Method declaration using the struct
	UFUNCTION()
	bool CheckForLedgeAbove(const FHitResult& InWallHit, FHitResult& OutLedgeHit, FLedgeDetectionData& OutData);
	
	UFUNCTION()
	FVector CalculateIdealHangPosition();

	UFUNCTION()
	bool IsCloseEnoughToHang() const;

private:
	//Ledge climbing related
	FHitResult WallHit;
	FHitResult LedgeHit;

	// Member variable
	FLedgeDetectionData LedgeData;
	
};

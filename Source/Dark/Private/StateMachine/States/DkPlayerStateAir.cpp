// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateAir.h"

void UDkPlayerStateAir::TickState()
{
	Super::TickState();
	if (CheckForWallInFront(WallHit))
	{
		if (CheckForLedgeAbove(WallHit, LedgeHit, LedgeData))
		{
			CalculateIdealHangPosition();
			if (IsCloseEnoughToHang())
			{
				// Ready to transition
				PlayerRef->StateManager->SwitchStateByKey("Hang");
			}
		}
	}
}

void UDkPlayerStateAir::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	LedgeData.Reset();
}

void UDkPlayerStateAir::OnStateExit()
{
	Super::OnStateExit();
}

bool UDkPlayerStateAir::CheckForWallInFront(FHitResult& OutHit)
{
	if (!PlayerRef) return false;

	//Start at aprox. chest height
	FVector PlayerLocation = PlayerRef->GetActorLocation();
	FVector TraceStart = PlayerLocation + FVector(0, 0, TraceStartHeightOffset);

	// Trace forward
	FVector TraceEnd = TraceStart + (PlayerRef->GetActorForwardVector() * TraceForWallDistance);

	//FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerRef);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility);

	if (bShowDebugVisuals)
	{
		// Debug visualization with color based on hit result
		DrawDebugLine(
			GetWorld(), 
			TraceStart, 
			bHitSomething ? OutHit.ImpactPoint : TraceEnd,  // Draw to hit point if we hit something
			bHitSomething ? FColor::Green : FColor::Red,
			false, 
			-1.0f, 
			0, 
			1.0f
		);

		// Add debug sphere only on hit
		if (bHitSomething)
		{
			DrawDebugSphere(
				GetWorld(),
				OutHit.ImpactPoint,
				25.0f,        // Radius
				12,           // Segments
				FColor::Green,
				false,        // Persistent
				-1.0f,        // Lifetime
				0,            // Depth Priority
				1.0f         // Thickness
			);
		}
	}
	return bHitSomething;
}

bool UDkPlayerStateAir::CheckForLedgeAbove(const FHitResult& InWallHit, FHitResult& OutLedgeHit, FLedgeDetectionData& OutData)
{
	if (!PlayerRef) return false;

	// Initialize detection data
	OutData.bIsValid = false;
	OutData.WallHitLocation = InWallHit.ImpactPoint;
	OutData.WallNormal = InWallHit.ImpactNormal;
	

	FVector BaseStart = WallHit.ImpactPoint +                  // Start at wall hit
						FVector(0, 0, LedgeCheckUpwardsOffset) +           // Go up
						(WallHit.ImpactNormal * -LedgeCheckInwardsOffset); // Step inward

	// Get right vector for side offsets (cross product of normal and up)
	FVector RightVector = FVector::CrossProduct(InWallHit.ImpactNormal, FVector::UpVector);

	// Define our three trace starts (center, left, right)
	FVector CenterStart = BaseStart;
	FVector LeftStart = BaseStart - (RightVector * LedgeCheckSideOffset);
	FVector RightStart = BaseStart + (RightVector * LedgeCheckSideOffset);

	// Define ends
	FVector CenterEnd = CenterStart + FVector(0, 0, -LedgeCheckDownwardTraceLength);
	FVector LeftEnd = LeftStart + FVector(0, 0, -LedgeCheckDownwardTraceLength);
	FVector RightEnd = RightStart + FVector(0, 0, -LedgeCheckDownwardTraceLength);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerRef);

	// Perform all three traces
	bool bHitCenter = GetWorld()->LineTraceSingleByChannel(OutLedgeHit, CenterStart, CenterEnd, ECC_Visibility);
	if (bHitCenter)
	{
		OutData.CenterHitLocation = OutLedgeHit.ImpactPoint;
	}
   
	FHitResult LeftHit, RightHit;
	bool bHitLeft = GetWorld()->LineTraceSingleByChannel(LeftHit, LeftStart, LeftEnd, ECC_Visibility);
	bool bHitRight = GetWorld()->LineTraceSingleByChannel(RightHit, RightStart, RightEnd, ECC_Visibility);

	if (bShowDebugVisuals)
	{
		// Debug visualization for all three traces
		// Center trace
		DrawDebugLine(
			GetWorld(), 
			CenterStart, 
			bHitCenter ? OutLedgeHit.ImpactPoint : CenterEnd,
			bHitCenter ? FColor::Green : FColor::Red,
			false, -1.0f, 0, 1.0f
		);

		// Left trace
		DrawDebugLine(
			GetWorld(), 
			LeftStart, 
			bHitLeft ? LeftHit.ImpactPoint : LeftEnd,
			bHitLeft ? FColor::Green : FColor::Red,
			false, -1.0f, 0, 1.0f
		);

		// Right trace
		DrawDebugLine(
			GetWorld(), 
			RightStart, 
			bHitRight ? RightHit.ImpactPoint : RightEnd,
			bHitRight ? FColor::Green : FColor::Red,
			false, -1.0f, 0, 1.0f
		);

		// Draw spheres at hit points
		if (bHitCenter)
		{
			DrawDebugSphere(GetWorld(), OutLedgeHit.ImpactPoint, 15.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
		}
		if (bHitLeft)
		{
			DrawDebugSphere(GetWorld(), LeftHit.ImpactPoint, 15.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
		}
		if (bHitRight)
		{
			DrawDebugSphere(GetWorld(), RightHit.ImpactPoint, 15.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
		}
	}

	// Count how many traces hit
	int32 HitCount = (bHitCenter ? 1 : 0) + (bHitLeft ? 1 : 0) + (bHitRight ? 1 : 0);

	// Quick returns for definite cases
	if (HitCount == 3)
	{
		OutData.bIsValid = true;
		return true;
	}
	if (HitCount < 2) return false;

	// If we get here, we know exactly 2 traces hit
	FVector MidStart, MidEnd;
	if (!bHitLeft)
	{
		// Calculate midpoint between center and left trace
		MidStart = (CenterStart + LeftStart) * 0.5f;
		MidEnd = MidStart + FVector(0, 0, -LedgeCheckDownwardTraceLength);
	}
	else if (!bHitRight)
	{
		// Calculate midpoint between center and right trace
		MidStart = (CenterStart + RightStart) * 0.5f;
		MidEnd = MidStart + FVector(0, 0, -LedgeCheckDownwardTraceLength);
	}

	FHitResult MidHit;
	bool bHitMid = GetWorld()->LineTraceSingleByChannel(MidHit, MidStart, MidEnd, ECC_Visibility);

	if (bShowDebugVisuals)
	{
		// Debug visualization for mid trace
		DrawDebugLine(
		   GetWorld(),
		   MidStart,
		   bHitMid ? MidHit.ImpactPoint : MidEnd,
		   bHitMid ? FColor::Yellow : FColor::Red,
		   false, -1.0f, 0, 1.0f
		);

		if (bHitMid)
		{
			DrawDebugSphere(GetWorld(), MidHit.ImpactPoint, 10.0f, 12, FColor::Yellow, false, -1.0f, 0, 1.0f);
		}
	}

	OutData.bIsValid = bHitMid;
	return bHitMid;
}

FVector UDkPlayerStateAir::CalculateIdealHangPosition()
{
	if (!LedgeData.bIsValid) return FVector::ZeroVector;
    
	LedgeData.IdealHangPosition = LedgeData.CenterHitLocation +                    
								 FVector(0, 0, HangPositionHeightOffset) +        
								 (LedgeData.WallNormal * HangPositionWallOffset);

	if (bShowDebugVisuals)
	{
		// Debug visualization
		DrawDebugSphere(
			GetWorld(),
			LedgeData.IdealHangPosition,
			15.0f,
			12,
			FColor::Purple,
			false,
			-1.0f,
			0,
			1.0f
		);
	}

	return LedgeData.IdealHangPosition;
}

bool UDkPlayerStateAir::IsCloseEnoughToHang() const
{
	if (!PlayerRef || !LedgeData.bIsValid) return false;

	FVector CurrentPosition = PlayerRef->GetActorLocation();
	float Distance = FVector::Dist(CurrentPosition, LedgeData.IdealHangPosition);

	if (bShowDebugVisuals)
	{
		DrawDebugSphere(
			GetWorld(),
			CurrentPosition,
			HangPositionTolerance,
			12,
			Distance <= HangPositionTolerance ? FColor::Red : FColor::White,
			false,
			-1.0f,
			0,
			1.0f
		);
	}

	return Distance <= HangPositionTolerance;
}
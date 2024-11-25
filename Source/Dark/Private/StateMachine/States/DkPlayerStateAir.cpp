// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateAir.h"

void UDkPlayerStateAir::TickState()
{
	Super::TickState();
	if (CheckForWallInFront(WallHit))
	{
		if (CheckForLedgeAbove(WallHit, LedgeHit))
		{
			
		}
	}
}

void UDkPlayerStateAir::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
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
	return bHitSomething;
}

bool UDkPlayerStateAir::CheckForLedgeAbove(const FHitResult& InWallHit, FHitResult& OutLedgeHit)
{
	if (!PlayerRef) return false;

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
   
	FHitResult LeftHit, RightHit;
	bool bHitLeft = GetWorld()->LineTraceSingleByChannel(LeftHit, LeftStart, LeftEnd, ECC_Visibility);
	bool bHitRight = GetWorld()->LineTraceSingleByChannel(RightHit, RightStart, RightEnd, ECC_Visibility);

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
		DrawDebugSphere(GetWorld(), OutLedgeHit.ImpactPoint, 20.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
	}
	if (bHitLeft)
	{
		DrawDebugSphere(GetWorld(), LeftHit.ImpactPoint, 20.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
	}
	if (bHitRight)
	{
		DrawDebugSphere(GetWorld(), RightHit.ImpactPoint, 20.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
	}

	// Count how many traces hit
	int32 HitCount = (bHitCenter ? 1 : 0) + (bHitLeft ? 1 : 0) + (bHitRight ? 1 : 0);
   
	// Return true if at least 2 traces hit
	return HitCount >= 2;
}

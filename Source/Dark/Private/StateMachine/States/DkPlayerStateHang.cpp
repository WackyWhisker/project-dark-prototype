// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateHang.h"

void UDkPlayerStateHang::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	// Cast to previous state to get ledge data
	if (UDkPlayerStateAir* PreviousAirState = Cast<UDkPlayerStateAir>(PreviousState))
	{
		LedgeData = PreviousAirState->GetLedgeData();
    
		if (PlayerRef && LedgeData.bIsValid)
		{
			// Initialize snap interpolation
			SnapStartPosition = PlayerRef->GetActorLocation();
			SnapTargetPosition = LedgeData.IdealHangPosition;
        
			SnapStartRotation = PlayerRef->GetActorRotation();
			SnapTargetRotation = FRotator(0, (-LedgeData.WallNormal).Rotation().Yaw, 0);
        
			SnapAlpha = 0.0f;
			bIsSnapping = true;

			// Lock movement
			if (UCharacterMovementComponent* MovementComp = PlayerRef->GetCharacterMovement())
			{
				MovementComp->SetMovementMode(MOVE_None);
				MovementComp->GravityScale = 0.0f;
				MovementComp->Velocity = FVector::ZeroVector;
			}
		}
	}

	if (!PlayerControllerRef) return;
	PlayerControllerRef->SetMappingContext("LedgeHang", true);
}

void UDkPlayerStateHang::TickState()
{
	Super::TickState();
	if (bIsSnapping && PlayerRef)
	{
		SnapAlpha = FMath::Min(SnapAlpha + GetWorld()->GetDeltaSeconds() * SnapSpeed, 1.0f);
    
		// Position
		FVector NewPosition = FMath::Lerp(SnapStartPosition, SnapTargetPosition, SnapAlpha);
    
		// Rotation (using same SnapAlpha)
		FRotator CurrentRotation = PlayerRef->GetActorRotation();
		FRotator TargetRotation = (-LedgeData.WallNormal).Rotation();
		TargetRotation.Pitch = CurrentRotation.Pitch;
		TargetRotation.Roll = CurrentRotation.Roll;
		FRotator NewRotation = FMath::Lerp(SnapStartRotation, TargetRotation, SnapAlpha);
    
		PlayerRef->SetActorLocation(NewPosition);
		PlayerRef->SetActorRotation(NewRotation);

		if (SnapAlpha >= 1.0f)
		{
			bIsSnapping = false;
		}
	}
}

void UDkPlayerStateHang::OnStateExit()
{
	Super::OnStateExit();
	if (!PlayerControllerRef) return;
	PlayerControllerRef->SetMappingContext("LedgeHang", false);
}


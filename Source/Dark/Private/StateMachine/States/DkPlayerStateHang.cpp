// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateHang.h"

#include "GameFramework/SpringArmComponent.h"

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
			ClimbAlpha = 0.0f;
			bIsSnapping = true;
			bIsClimbing = false;

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

   if (bIsClimbing && PlayerRef)
   {
   	ClimbAlpha = FMath::Min(ClimbAlpha + GetWorld()->GetDeltaSeconds() * ClimbSpeed, 1.0f);
    
   	// Calculate arc-like movement
   	float VerticalAlpha = FMath::Sin(ClimbAlpha * PI * 0.5f);   
   	float HorizontalAlpha = FMath::Pow(ClimbAlpha, 3.0f);       // Changed from Square to Pow(x,3)
    
   	FVector VerticalOffset = FVector(0, 0, 1) * ClimbHeightCurve * VerticalAlpha;
   	FVector HorizontalOffset = LedgeData.WallNormal * ClimbForwardCurve * HorizontalAlpha;
    
   	FVector NewPosition = ClimbStartPosition + VerticalOffset + HorizontalOffset;
   	NewPosition = FMath::Lerp(NewPosition, ClimbTargetPosition, ClimbAlpha);

       if (bShowDebugVisuals)
       {
           DrawDebugSphere(
               GetWorld(),
               NewPosition,
               15.0f,
               12,
               FColor::Yellow,
               true,
               -1.0f,
               0,
               1.0f
           );
       }

       PlayerRef->SetActorLocation(NewPosition);

       if (ClimbAlpha >= 1.0f)
       {
           bIsClimbing = false;
           PlayerRef->StateManager->SwitchStateByKey("Idle");
       		if (PlayerControllerRef->PlayerSpringArmRef)
       		{
       			PlayerControllerRef->PlayerSpringArmRef->bDoCollisionTest = true;
       		}
       }
   }
}

void UDkPlayerStateHang::Drop()
{
	Super::Drop();
	if (!PlayerRef) return;
	PlayerRef->StateManager->SwitchStateByKey("Fall");
}

void UDkPlayerStateHang::Lift()
{
	Super::Lift();
	if (!PlayerRef || !LedgeData.bIsValid) return;

	ClimbStartPosition = PlayerRef->GetActorLocation();
	ClimbTargetPosition = LedgeData.ClimbUpPosition;
	ClimbAlpha = 0.0f;
	bIsClimbing = true;

	if (PlayerControllerRef->PlayerSpringArmRef)
	{
		PlayerControllerRef->PlayerSpringArmRef->bDoCollisionTest = false;
	}
}

void UDkPlayerStateHang::OnStateExit()
{
	Super::OnStateExit();
	if (!PlayerControllerRef) return;
	PlayerControllerRef->SetMappingContext("LedgeHang", false);

	// Unlock movement
	if (UCharacterMovementComponent* MovementComp = PlayerRef->GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Falling);
		MovementComp->GravityScale = 1.0f;
	}
}


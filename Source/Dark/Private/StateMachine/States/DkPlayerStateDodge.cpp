// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateDodge.h"

void UDkPlayerStateDodge::TickState()
{
	Super::TickState();
	if (!bHasLaunched || !bCanTransition) {return;}
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

void UDkPlayerStateDodge::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	DodgeLaunchCharacter();
	// Set minimum time before state can transition using the configurable property
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, 
		this, &UDkPlayerStateDodge::EnableStateTransition, MinStateTransitionTime, false);
	bCanTransition = false;
}

void UDkPlayerStateDodge::OnStateExit()
{
	Super::OnStateExit();
	bHasLaunched = false;
}

void UDkPlayerStateDodge::DodgeLaunchCharacter()
{
	FVector PlayerVelocity = PlayerRef->GetCharacterMovement()->Velocity;
	FVector FloorNormal;
	FHitResult HitResult;
	FVector Start = PlayerRef->GetActorLocation();
	FVector End = Start - FVector(0, 0, 100);  // Trace downward
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerRef);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		FloorNormal = HitResult.Normal;
		FVector LaunchDir = FVector::VectorPlaneProject(PlayerVelocity, FloorNormal).GetSafeNormal();
		FVector LaunchVelocity = (LaunchDir * DodgeLaunchStrength) + FVector(0, 0, DodgeZStrength);
		PlayerRef->LaunchCharacter(LaunchVelocity, true, true);
		bHasLaunched = true;
	}
}

void UDkPlayerStateDodge::EnableStateTransition()
{
	bCanTransition = true;
}

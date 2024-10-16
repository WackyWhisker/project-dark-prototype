// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateFall.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateFall::TickState()
{
	Super::TickState();
	if (IsNearGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Land");
	}
	//TODO: CHECK IF CLEAN ENOUGH. EDGE CASE IN CASE LANDING CHECK IS WONKY
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

void UDkPlayerStateFall::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Fall;
}

bool UDkPlayerStateFall::IsNearGround()
{
	FVector Start = PlayerRef->GetActorLocation();
	FVector End = Start + PlayerRef->GetActorUpVector() * -500.0f;
	const FName TraceTag("GroundCheckTraceTag");

	GetWorld()->DebugDrawTraceTag = TraceTag;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.TraceTag = TraceTag;
	Params.AddIgnoredActor(PlayerRef);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params, FCollisionResponseParams()))
	{
		if (HitResult.bBlockingHit && HitResult.Distance <= NearGroundDistance)
		{
			return true;
		}
		return false;
	}
	return false;
}

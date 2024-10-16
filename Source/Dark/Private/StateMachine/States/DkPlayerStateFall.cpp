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
		if (HitResult.bBlockingHit && HitResult.Distance <= 150.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

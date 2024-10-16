// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateIdle.h"

void UDkPlayerStateIdle::TickState()
{
	Super::TickState();
	//UE_LOG(LogTemp, Warning, TEXT("Idle State Ticking"));
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
	if (!PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Fall");
	}
}

void UDkPlayerStateIdle::Jump()
{
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Executing Jump in Idle State"));
	PlayerRef->Jump();
	PlayerRef->StateManager->SwitchStateByKey("Jump");
}

void UDkPlayerStateIdle::Attack()
{
	Super::Attack();
	UE_LOG(LogTemp, Warning, TEXT("Executing Attack in Idle State"));
	PlayerRef->StateManager->SwitchStateByKey("Attack");
}

void UDkPlayerStateIdle::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Idle;
}

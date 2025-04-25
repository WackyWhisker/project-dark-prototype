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
	if (!PlayerRef->GetCharacterMovement()->IsMovingOnGround() && (!PlayerRef->StateManager->CurrentState->PreviousState || PlayerRef->StateManager->CurrentState->PreviousState->StateDisplayName != "Hang"))
	{
		PlayerRef->StateManager->SwitchStateByKey("Fall");
	}
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

	PlayerRef->GetCharacterMovement()->RotationRate = FRotator(0, 500.0f, 0.0f);
	PlayerRef->GetCharacterMovement()->GravityScale = 1.0f;
}

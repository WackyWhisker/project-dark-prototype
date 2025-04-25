// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateRun.h"

void UDkPlayerStateRun::TickState()
{
	Super::TickState();
	//UE_LOG(LogTemp, Warning, TEXT("Run State Ticking"));
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}

	if (!PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Fall");
	}
}


void UDkPlayerStateRun::Attack()
{
	Super::Attack();
	UE_LOG(LogTemp, Warning, TEXT("Executing Attack in Run State"));
	PlayerRef->StateManager->SwitchStateByKey("Attack");
}

void UDkPlayerStateRun::Dodge()
{
	Super::Dodge();
	UE_LOG(LogTemp, Warning, TEXT("Executing Dodge in Run State"));
	PlayerRef->StateManager->SwitchStateByKey("Dodge");
}

void UDkPlayerStateRun::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Run;
}

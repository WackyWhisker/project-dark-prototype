// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateJump.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateJump::TickState()
{
	Super::TickState();
	if (PlayerRef->GetCharacterMovement()->Velocity.Z < 0)
	{
		PlayerRef->StateManager->SwitchStateByKey("Fall");
	}
}

void UDkPlayerStateJump::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Jump;
}

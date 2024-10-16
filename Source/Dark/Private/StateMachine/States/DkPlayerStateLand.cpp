// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateLand.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateLand::TickState()
{
	Super::TickState();
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

void UDkPlayerStateLand::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Land;
}

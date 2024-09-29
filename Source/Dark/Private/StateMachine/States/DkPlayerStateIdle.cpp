// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateIdle.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		PlayerRef->StateManager->SwitchStateByKey("Air");
	}
}

void UDkPlayerStateIdle::Jump()
{
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Executing Jump in Idle State"));
	PlayerRef->Jump();
}

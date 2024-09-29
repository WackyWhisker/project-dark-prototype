// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateAir.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateAir::TickState()
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

void UDkPlayerStateAir::Jump()
{
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Cannot Jump again"));
}

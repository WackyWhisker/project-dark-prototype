// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateJump.h"

void UDkPlayerStateJump::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Jump;
	PlayerRef->GetCharacterMovement()->RotationRate = FRotator(0, AirYaw, 0.0f);
}

void UDkPlayerStateJump::TickState()
{
	Super::TickState();
	if (PlayerRef->GetCharacterMovement()->Velocity.Z <= 0)
	{
		PlayerRef->StateManager->SwitchStateByKey("Fall");
	}
}

void UDkPlayerStateJump::Jump()
{
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Executing Jump in again in jump State"));
	PlayerRef->Jump();
	PlayerRef->StateManager->SwitchStateByKey("Jump");
}


// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		PlayerRef->StateManager->SwitchStateByKey("Air");
	}
}

void UDkPlayerStateRun::Jump()
{
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Executing Jump in Run State"));
	PlayerRef->Jump();
}

void UDkPlayerStateRun::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Run;
}

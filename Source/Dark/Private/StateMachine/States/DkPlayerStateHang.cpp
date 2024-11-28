// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateHang.h"

void UDkPlayerStateHang::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	//TEMP
	if (!PlayerRef) return;

	PlayerRef->GetCharacterMovement()->SetMovementMode(MOVE_None);
    PlayerRef->GetCharacterMovement()->GravityScale = 0.0f;
}

void UDkPlayerStateHang::TickState()
{
	Super::TickState();
}

void UDkPlayerStateHang::OnStateExit()
{
	Super::OnStateExit();
}


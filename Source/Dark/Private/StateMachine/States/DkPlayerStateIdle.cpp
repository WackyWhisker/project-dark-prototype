// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateIdle.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateIdle::TickState()
{
	Super::TickState();
	UE_LOG(LogTemp, Warning, TEXT("Idle State Ticking"));
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f)
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

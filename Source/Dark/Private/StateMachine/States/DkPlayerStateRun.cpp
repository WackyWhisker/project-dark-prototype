// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDkPlayerStateRun::TickState()
{
	Super::TickState();
	UE_LOG(LogTemp, Warning, TEXT("Run State Ticking"));
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f)
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
}

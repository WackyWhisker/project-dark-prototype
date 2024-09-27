// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateBase.h"

void UDkPlayerStateBase::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(StateOwner);
	}
}

void UDkPlayerStateBase::OnStateExit()
{
	Super::OnStateExit();
}

// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerControllerInterface.h"

void UDkPlayerStateBase::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(StateOwner);
	}

	if (!PlayerController)
	{
		PlayerController = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	}

	//Bind all relevant delegates
	//TODO: Consider which of these should be in the child states only
	if(PlayerController)
	{
		PlayerController->GetJumpDelegate()->AddUObject(this, &UDkPlayerStateBase::Jump);
		PlayerController->GetAttackDelegate()->AddUObject(this, &UDkPlayerStateBase::Attack);
	}
}

void UDkPlayerStateBase::OnStateExit()
{
	Super::OnStateExit();
	//Unbind all relevant delegates
	if(PlayerController)
	{
		PlayerController->GetJumpDelegate()->RemoveAll(this);
		PlayerController->GetAttackDelegate()->RemoveAll(this);
	}
}

void UDkPlayerStateBase::Jump()
{
	//executed in child states
}

void UDkPlayerStateBase::Attack()
{
	
}

// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateAttack.h"

void UDkPlayerStateAttack::TickState()
{
	Super::TickState();
}

void UDkPlayerStateAttack::Attack()
{
	Super::Attack();
}

void UDkPlayerStateAttack::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Attack;
	UAnimInstance* AnimInstance = PlayerRef->GetMesh()->GetAnimInstance();
	
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
	UE_LOG(LogTemp, Warning, TEXT("Attack Index: %s"), *FString::FromInt(AttackIndex));
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

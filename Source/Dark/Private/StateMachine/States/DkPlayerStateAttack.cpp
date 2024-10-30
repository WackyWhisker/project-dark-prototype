// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateAttack.h"

void UDkPlayerStateAttack::TickState()
{
	Super::TickState();
	if (IsAttacking) {return;}
	
	if (PlayerRef->GetCharacterMovement()->Velocity.Length() == 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Idle");
	}
	else if (PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.0f && PlayerRef->GetCharacterMovement()->IsMovingOnGround())
	{
		PlayerRef->StateManager->SwitchStateByKey("Run");
	}
}

void UDkPlayerStateAttack::Attack()
{
	Super::Attack();
}

void UDkPlayerStateAttack::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	IsAttacking = true;
	PlayerRef->DkPlayerState = EDkPlayerAnimationState::Attack;
	AnimInstance = PlayerRef->GetMesh()->GetAnimInstance();
	
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UDkPlayerStateAttack::OnMontageEnded);
		AnimInstance->Montage_Play(AttackMontage);
	}
	UE_LOG(LogTemp, Warning, TEXT("Attack Index: %s"), *FString::FromInt(AttackIndex));
	AttackIndex++;
	
}

void UDkPlayerStateAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveAll(this);
	}
}

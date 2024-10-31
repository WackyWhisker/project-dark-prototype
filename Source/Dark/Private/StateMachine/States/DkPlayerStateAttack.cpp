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

	//Stop player movement and decelerate
	PlayerPCRef->SetIgnoreMoveInput(true);
	PreviousBrakingFrictionFactor = PlayerRef->GetCharacterMovement()->BrakingFrictionFactor;
	PreviousBrakingDecelerationWalking = PlayerRef->GetCharacterMovement()->BrakingDecelerationWalking;
	PlayerRef->GetCharacterMovement()->BrakingFrictionFactor = 1.5f;  // Increase for more friction
	PlayerRef->GetCharacterMovement()->BrakingDecelerationWalking = 100.0f;
	
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

	//Re-enable player movement and restore deceleration values
	PlayerPCRef->SetIgnoreMoveInput(false);
	PlayerRef->GetCharacterMovement()->BrakingFrictionFactor = PreviousBrakingFrictionFactor;
	PlayerRef->GetCharacterMovement()->BrakingDecelerationWalking = PreviousBrakingDecelerationWalking;
}

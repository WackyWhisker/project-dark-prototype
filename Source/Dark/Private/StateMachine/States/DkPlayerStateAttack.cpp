// Copyright @ Christian Reichel

#include "StateMachine/States/DkPlayerStateAttack.h"

void UDkPlayerStateAttack::TickState()
{
    Super::TickState();
    if (IsAttacking) { return; }

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
    
    if (!IsAttacking)
    {
        IsAttacking = true;
        PlayerRef->DkPlayerState = EDkPlayerAnimationState::Attack;
        AnimInstance = PlayerRef->GetMesh()->GetAnimInstance();
        
        if (AnimInstance && AttackMontage)
        {
            AnimInstance->OnMontageEnded.AddDynamic(this, &UDkPlayerStateAttack::OnMontageEnded);
            AnimInstance->Montage_Play(AttackMontage);
            AnimInstance->Montage_JumpToSection(FirstAttackSection, AttackMontage);
        }
    }
    else if (IsInAttackWindow && AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_JumpToSection(SecondAttackSection, AttackMontage);
        IsInAttackWindow = false; // Prevent further attacks in this combo
    }
}

void UDkPlayerStateAttack::OnStateEnter(AActor* StateOwner)
{
    Super::OnStateEnter(StateOwner);
    PlayerRef->AttachWeaponToSocket("AttackWeaponSocket");
    Attack();
}

void UDkPlayerStateAttack::OnStateExit()
{
    Super::OnStateExit();
    PlayerRef->AttachWeaponToSocket("HolsterWeaponSocket");
    
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.RemoveAll(this);
    }
}

void UDkPlayerStateAttack::OpenAttackWindow()
{
    IsInAttackWindow = true;
}

void UDkPlayerStateAttack::CloseAttackWindow()
{
    IsInAttackWindow = false;
    
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Stop(0.25f, AttackMontage);
    }
}

void UDkPlayerStateAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    IsAttacking = false;
    IsInAttackWindow = false;
    
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.RemoveAll(this);
    }
}
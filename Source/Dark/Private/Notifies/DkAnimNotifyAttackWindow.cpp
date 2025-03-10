// Copyright @ Christian Reichel

#include "Notifies/DkAnimNotifyAttackWindow.h"
#include "Character/DkCharacter.h"
#include "StateMachine/DkStateManagerComponent.h"
#include "StateMachine/States/DkPlayerStateAttack.h"

void UDkAnimNotifyAttackWindow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (ADkCharacter* Character = Cast<ADkCharacter>(MeshComp->GetOwner()))
	{
		if (UDkStateManagerComponent* StateManager = Character->StateManager)
		{
			if (UDkPlayerStateAttack* AttackState = Cast<UDkPlayerStateAttack>(StateManager->CurrentState))
			{
				if (WindowAction == EAttackWindowAction::Open)
				{
					AttackState->OpenAttackWindow();
				}
				else
				{
					AttackState->CloseAttackWindow();
				}
			}
		}
	}
}
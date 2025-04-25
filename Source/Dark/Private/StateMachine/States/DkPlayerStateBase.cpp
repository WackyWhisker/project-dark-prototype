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
       if (!PlayerControllerRef)
       {
          PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
       }
    }

    if (!PlayerControllerInterface)
    {
       PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
    }

    //Bind all relevant delegates
    //TODO: Consider which of these should be in the child states only
    if(PlayerControllerInterface)
    {
       PlayerControllerInterface->GetAttackDelegate()->AddUObject(this, &UDkPlayerStateBase::Attack);
       PlayerControllerInterface->GetDodgeDelegate()->AddUObject(this, &UDkPlayerStateBase::Dodge);
       PlayerControllerInterface->GetLiftDelegate()->AddUObject(this, &UDkPlayerStateBase::Lift);
       PlayerControllerInterface->GetDropDelegate()->AddUObject(this, &UDkPlayerStateBase::Drop);
    }
}

void UDkPlayerStateBase::OnStateExit()
{
    Super::OnStateExit();
    //Unbind all relevant delegates
    if(PlayerControllerInterface)
    {
       PlayerControllerInterface->GetAttackDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetDodgeDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetLiftDelegate()->RemoveAll(this);
       PlayerControllerInterface->GetDropDelegate()->RemoveAll(this);
    }
}

void UDkPlayerStateBase::Attack()
{
    //executed in child states
}

void UDkPlayerStateBase::Dodge()
{
    //executed in child states
}

void UDkPlayerStateBase::Lift()
{
    //executed in child states
}

void UDkPlayerStateBase::Drop()
{
    //executed in child states
}
// Copyright @ Christian Reichel


#include "Character/DkCharacterBase.h"

ADkCharacterBase::ADkCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADkCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADkCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADkCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Copyright @ Christian Reichel


#include "Character/DarkCharacterBase.h"

ADarkCharacterBase::ADarkCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADarkCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADarkCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADarkCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


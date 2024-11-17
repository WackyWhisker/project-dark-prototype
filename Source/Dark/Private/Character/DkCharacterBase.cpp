// Copyright @ Christian Reichel


#include "Character/DkCharacterBase.h"

ADkCharacterBase::ADkCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetNestedDefaultSubobjectClass<UDkCharacterMovementComponent>(TEXT("CharMoveComp")))
{
	// Any base character initialization
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


// Copyright @ Christian Reichel


#include "Enemy/DkEnemyBase.h"

ADkEnemyBase::ADkEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADkEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADkEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//IDkTargetableInterface functions
bool ADkEnemyBase::CanBeTargeted_Implementation() const
{
	return true; //TODO: implement conditions for enemies to be tartetable
}

void ADkEnemyBase::OnTargeted_Implementation()
{
	
}

void ADkEnemyBase::OnUntargeted_Implementation()
{
}

FVector ADkEnemyBase::GetTargetLocation_Implementation() const
{
	return FVector::ZeroVector;
}



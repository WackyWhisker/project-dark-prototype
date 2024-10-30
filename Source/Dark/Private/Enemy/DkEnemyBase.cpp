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



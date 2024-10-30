// Copyright @ Christian Reichel


#include "Enemy/DkEnemy.h"

ADkEnemy::ADkEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADkEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADkEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


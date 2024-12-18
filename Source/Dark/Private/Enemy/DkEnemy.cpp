// Copyright @ Christian Reichel


#include "Enemy/DkEnemy.h"

ADkEnemy::ADkEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADkEnemy::HandleDeath_Implementation()
{
	IDkDeathHandlerInterface::HandleDeath_Implementation();
	Destroy(); //TODO: Replace with proper enemy death sequence
}

void ADkEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADkEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


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

bool ADkEnemyBase::CanBeTargeted_Implementation() const
{
	return IDkTargetableInterface::CanBeTargeted_Implementation();
}

void ADkEnemyBase::OnTargeted_Implementation()
{
	IDkTargetableInterface::OnTargeted_Implementation();
	UE_LOG(LogTemp, Warning, TEXT("It was I, %s, being chosen as target"), *GetName());
	DrawDebugSphere(
	GetWorld(),           // World context
	GetActorLocation(),      // Center point
	100.0f,              // Radius
	32,                  // Segments
	FColor::Red,         // Color
	false,               // Persistent lines
	1.0f,              // Life time (if temporary)
	0,                   // Depth priority
	1.0f                 // Thickness
);
}

void ADkEnemyBase::OnUntargeted_Implementation()
{
	IDkTargetableInterface::OnUntargeted_Implementation();
}

FVector ADkEnemyBase::GetTargetLocation_Implementation() const
{
	return IDkTargetableInterface::GetTargetLocation_Implementation();
}





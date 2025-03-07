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
    
	StartLocation = GetActorLocation();
	GenerateNewTargetPoint();
}

void ADkEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (HasReachedTarget())
	{
		GenerateNewTargetPoint();
	}
    
	MoveTowardsTarget(DeltaTime);
}

void ADkEnemy::GenerateNewTargetPoint()
{
	// Generate random point within circle
	float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
	float RandomRadius = FMath::RandRange(0.0f, MovementRadius);
    
	FVector RandomOffset(
	   RandomRadius * FMath::Cos(RandomAngle),
	   RandomRadius * FMath::Sin(RandomAngle),
	   0.0f
	);
    
	CurrentTargetPoint = StartLocation + RandomOffset;
}

void ADkEnemy::MoveTowardsTarget(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToTarget = (CurrentTargetPoint - CurrentLocation).GetSafeNormal();
    
	// Calculate new position
	FVector NewLocation = CurrentLocation + DirectionToTarget * MovementSpeed * DeltaTime;
    
	// Calculate target rotation
	FRotator TargetRotation = DirectionToTarget.Rotation();
    
	// Smoothly interpolate current rotation to target rotation
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationSpeed
	);
    
	// Update position and rotation
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
}

bool ADkEnemy::HasReachedTarget() const
{
	return FVector::Distance(GetActorLocation(), CurrentTargetPoint) < AcceptanceRadius;
}
// Copyright @ Christian Reichel


#include "Component/DkCharacterMovementComponent.h"


// Sets default values for this component's properties
UDkCharacterMovementComponent::UDkCharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


float UDkCharacterMovementComponent::GetMaxSpeed() const
{
	FVector ForwardVector = GetOwner()->GetActorForwardVector();
	FVector VelocityNormal = Velocity.GetSafeNormal();
   
	// Dot product gives us how "forward" we're moving (-1 to 1)
	float ForwardnessDot = FVector::DotProduct(VelocityNormal, ForwardVector);
   
	// Get absolute value of dot product for easier comparison
	float ForwardAmount = FMath::Abs(ForwardnessDot);
   
	// If we're moving mostly forward (within 45 degrees of forward vector)
	if (ForwardAmount > 0.7f && ForwardnessDot > 0)  // 0.7 is roughly cos(45°)
	{
		return Super::GetMaxSpeed();  // Full forward speed
	}
   
	// Otherwise (moving backward or sideways)
	return MaxNonForwardSpeed;
}

// Called when the game starts
void UDkCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDkCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


// Copyright @ Christian Reichel


#include "Component/DkCharacterMovementComponent.h"

UDkCharacterMovementComponent::UDkCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDkCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}


void UDkCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


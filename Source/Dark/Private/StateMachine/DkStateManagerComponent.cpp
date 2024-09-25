// Copyright @ Christian Reichel

#include "StateMachine/DkStateManagerComponent.h"

UDkStateManagerComponent::UDkStateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDkStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDkStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDkStateManagerComponent::SwitchStateByKey(FString StateKey)
{
}

void UDkStateManagerComponent::SwitchStateByObject(UDkStateBase* StateObject)
{
}

void UDkStateManagerComponent::InitializeStateManager()
{
}

void UDkStateManagerComponent::InitializeStates()
{
}


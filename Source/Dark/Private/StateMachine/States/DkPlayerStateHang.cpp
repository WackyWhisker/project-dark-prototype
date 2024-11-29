// Copyright @ Christian Reichel


#include "StateMachine/States/DkPlayerStateHang.h"

void UDkPlayerStateHang::OnStateEnter(AActor* StateOwner)
{
	Super::OnStateEnter(StateOwner);
	// Cast to previous state to get ledge data
	if (UDkPlayerStateAir* PreviousAirState = Cast<UDkPlayerStateAir>(PreviousState))
	{
		LedgeData = PreviousAirState->GetLedgeData();
		UE_LOG(LogTemp, Warning, TEXT("Hang state entered and ledge data stored."));
	}
}

void UDkPlayerStateHang::TickState()
{
	Super::TickState();
}

void UDkPlayerStateHang::OnStateExit()
{
	Super::OnStateExit();
}


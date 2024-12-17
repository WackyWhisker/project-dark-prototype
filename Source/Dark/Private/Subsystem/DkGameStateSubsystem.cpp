// Copyright @ Christian Reichel


#include "Subsystem/DkGameStateSubsystem.h"

UDkGameStateSubsystem::UDkGameStateSubsystem()
{
	CurrentGameState = EDkGameState::Playing;
}

bool UDkGameStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		UE_LOG(LogTemp, Log, TEXT("DkGameStateSubsystem: Checking creation for world: %s"), *World->GetName());

		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			UE_LOG(LogTemp, Log, TEXT("DkGameStateSubsystem: Found GameMode: %s"), *GameMode->GetClass()->GetName());
			return GameMode->IsA<ADkGameModeBase>();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("DkGameStateSubsystem: No GameMode found yet"));
			// More permissive: Allow creation in game worlds even without GameMode
			return World->WorldType == EWorldType::Game || 
				   World->WorldType == EWorldType::PIE;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("DkGameStateSubsystem: No valid World found"));
	return false;
}

void UDkGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
    
	if (const UWorld* World = GetWorld())
	{
		UE_LOG(LogTemp, Log, TEXT("DkGameStateSubsystem: Successfully initialized in world %s"), *World->GetName());
	}
}

bool UDkGameStateSubsystem::RequestStateChange(EDkGameState NewState)
{
	if (!IsValidStateTransition(NewState))
	{
		return false;
	}

	EDkGameState OldState = CurrentGameState;
	CurrentGameState = NewState;
    
	HandleStateChanged(NewState);
	OnGameStateChanged.Broadcast(NewState, OldState);
    
	return true;
}

void UDkGameStateSubsystem::BeginDeathSequence()
{
	if (CurrentGameState == EDkGameState::Playing)
	{
		RequestStateChange(EDkGameState::Dying);
	}
}

bool UDkGameStateSubsystem::IsValidStateTransition(EDkGameState NewState) const
{
	switch (CurrentGameState)
	{
	case EDkGameState::Playing:
		return NewState == EDkGameState::Dying;
            
	case EDkGameState::Dying:
		return NewState == EDkGameState::Resetting;
            
	case EDkGameState::Resetting:
		return NewState == EDkGameState::Respawning;
            
	case EDkGameState::Respawning:
		return NewState == EDkGameState::Playing;
            
	default:
		return false;
	}
}

void UDkGameStateSubsystem::HandleStateChanged(EDkGameState NewState)
{
	if (bShowDebugState && GEngine)
	{
		const FString StateText = FString::Printf(TEXT("Game State Changed: %s"), 
			*UEnum::GetValueAsString(NewState));
            
		GEngine->AddOnScreenDebugMessage(
			-1,             // Key (-1 for unique)
			15.0f,          // Duration
			FColor::Green,
			StateText);
	}

	switch (NewState)
	{
	case EDkGameState::Dying:
		// Initialize death sequence
			break;
            
	case EDkGameState::Resetting:
		// Begin level reset
			break;
            
	case EDkGameState::Respawning:
		// Prepare for respawn
			break;
            
	case EDkGameState::Playing:
		// Resume normal gameplay
			break;
	}
}

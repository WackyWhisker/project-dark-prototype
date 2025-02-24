// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/DkGameModeBase.h"
#include "DkGameStateSubsystem.generated.h"

UENUM()
enum class EDkGameState : uint8
{
	Playing,
	Dying,
	Retreat,
	Resetting,
	Respawning
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDkGameStateChanged, EDkGameState, NewState, EDkGameState, OldState);

UCLASS()
class DARK_API UDkGameStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UDkGameStateSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// State getters/setters
	UFUNCTION(BlueprintPure, Category = "Dk|GameState")
	EDkGameState GetCurrentGameState() const { return CurrentGameState; }
	
	// Try to transition to a new state. Returns false if transition is invalid
	UFUNCTION(BlueprintCallable, Category = "Dk|GameState")
	bool RequestStateChange(EDkGameState NewState);

	// Start the death sequence. Called by health component when health reaches 0
	UFUNCTION(BlueprintCallable, Category = "Dk|GameState")
	void BeginDeathSequence();

	// Start the retreat sequence. Similar to death sequence but for tactical retreat
	UFUNCTION(BlueprintCallable, Category = "Dk|GameState")
	void BeginRetreatSequence();

	// State change broadcast
	UPROPERTY(BlueprintAssignable, Category = "Dk|GameState")
	FOnDkGameStateChanged OnGameStateChanged;

	// Register anything that needs to be part of the reset process
	void RegisterForReset(UObject* Object);
    
	// Called when any registered object completes its reset
	void NotifyResetComplete(UObject* Object);
    
	// Helper to check if we're still waiting for resets
	bool AreAnyResetsPending() const;

protected:
	// Validates if a state transition is allowed
	bool IsValidStateTransition(EDkGameState NewState) const;
    
	// Internal state change handler
	void HandleStateChanged(EDkGameState NewState);
	
	UPROPERTY()
	EDkGameState CurrentGameState = EDkGameState::Playing;
	
	UPROPERTY(EditAnywhere, Category = "Dk|Debug")
	bool bShowDebugState = false;
	
private:
	// Track anything that needs to complete before reset is done
	TSet<TWeakObjectPtr<UObject>> PendingResetObjects;
};


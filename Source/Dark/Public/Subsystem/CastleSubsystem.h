// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/World.h"
#include "CastleSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CastleLog, Log, All);

class UCastleRoomData;
class ACastleWorldSettings;

UCLASS()
class DARK_API UCastleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/*--Public Methods --------------------*/

	// Initialize the subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Simple function we can call to verify subsystem is accessible
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void TestSubsystemAccess();
	
	// Load a random available room
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void LoadRandomRoom();

	// Load a specific room level
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	void LoadRoom(FName RoomLevelName);

	// Unload a specific room level
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	void UnloadRoom(FName RoomLevelName);

	// Check if a room is currently loaded
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	bool IsRoomLoaded(FName RoomLevelName);

public:
	/*--Public Properties ----------------*/

protected:
	/*--Protected Methods ---------------*/
	
	// Callback for when room finishes loading
	UFUNCTION()
	void OnRoomLoaded();

	// Callback for when room finishes unloading
	UFUNCTION()
	void OnRoomUnloaded();

protected:
	/*--Protected Properties ------------*/
	
	//Reference to our Castle Room Settings
	UPROPERTY(EditDefaultsOnly, Category = "Castle Configuration")
	TObjectPtr<ACastleWorldSettings> CastleWorldSettings;
	
	// Track our active rooms
	UPROPERTY()
	TArray<FName> LoadedRooms;

private:
	/*--Private Methods ----------------*/

private:
	/*--Private Properties -------------*/
};
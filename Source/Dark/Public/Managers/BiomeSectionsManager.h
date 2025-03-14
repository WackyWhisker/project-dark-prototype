// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/CastleRoomData.h"
#include "BiomeSectionsManager.generated.h"

class UCastleRoomData;
class ACastleSocketActor;
class UDkGameStateSubsystem;

USTRUCT(BlueprintType)
struct FSocketTransform
{
	GENERATED_BODY()

	// The socket's transform in the level
	UPROPERTY()
	FTransform Transform;

	// The socket's name
	UPROPERTY()
	FName SocketName;

	FSocketTransform()
		: Transform(FTransform::Identity)
		, SocketName(NAME_None)
	{}
};

USTRUCT(BlueprintType)
struct FLoadedRoomInfo
{
	GENERATED_BODY()

	FString RoomID;
	FName LevelName;
	ECastleRoomType RoomType;
};

USTRUCT()
struct FPendingRoomConnection
{
	GENERATED_BODY()

	FString RoomID;
	FString ConnectedToRoomID;
	FName ConnectFromSocket;
	FName ConnectToSocket;

	FPendingRoomConnection() {}
	FPendingRoomConnection(FString InRoomID, FString InConnectedToRoomID, 
		FName InConnectFromSocket, FName InConnectToSocket)
		: RoomID(InRoomID)
		, ConnectedToRoomID(InConnectedToRoomID)
		, ConnectFromSocket(InConnectFromSocket)
		, ConnectToSocket(InConnectToSocket)
	{}
};

USTRUCT()
struct FRoomLoadingInfo
{
	GENERATED_BODY()

	FString RoomID;
	FName LevelName;
	ECastleRoomType RoomType;
	FString ConnectedToRoomID;
	FName ConnectFromSocket;
	FName ConnectToSocket;

	FRoomLoadingInfo()
		: RoomType(ECastleRoomType::None)
	{}
};

UCLASS()
class DARK_API ABiomeSectionsManager : public AActor
{
	GENERATED_BODY()

public:
	ABiomeSectionsManager();

	virtual void Tick(float DeltaTime) override;

	// Main function to spawn the entire dungeon
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void SpawnCompleteCastle();
	void ProcessNextRoom();

	// Load the initial entry room
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void LoadEntryRoom();

	// Load a random connection room
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void LoadRandomConnectionRoom(const FString& ConnectedToRoomID);

	// Load a specific room level with optional connection info
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	void LoadRoom(const FString& RoomID, const FString& ConnectedToRoomID = "", 
		FName ConnectFromSocket = NAME_None, FName ConnectToSocket = NAME_None);

	// Unload a specific room
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	void UnloadRoom(const FString& RoomID);

	// Check if a room is currently loaded
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	bool IsRoomLoaded(const FString& RoomID) const;

	// Get all currently loaded rooms
	UFUNCTION(BlueprintCallable, Category = "Castle Rooms")
	TArray<FString> GetLoadedRoomIDs() const;
   
	void ProcessPendingConnections();

	// Completely clears the castle - unloads all rooms and resets internal state
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void ClearCompleteCastle();

	UFUNCTION()
	void OnRoomLoaded();

	UFUNCTION()
	void OnRoomUnloaded();

	// Helper function to find entry point
	FString FindEntryRoomID() const;
    
	// Helper function to get next rooms in sequence
	TArray<FString> GetNextRooms(const FString& CurrentRoomID) const;

	// Calculate transform for connecting rooms
	FTransform CalculateRoomTransform(const FString& SourceRoomID, 
		const FString& TargetRoomID, FName SourceSocket, FName TargetSocket);

	// Helper to get streaming level by name
	ULevelStreaming* GetLevelByName(const FName& LevelName) const;

	// Helper to check if any rooms are still in process of being unloaded
	bool AreAnyRoomsUnloading() const;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Castle Configuration")
	TObjectPtr<UCastleRoomData> RoomData;
    
	// Track loaded rooms with their info
	UPROPERTY()
	TMap<FString, FLoadedRoomInfo> LoadedRooms;

	UPROPERTY()
	TArray<FPendingRoomConnection> PendingConnections;
    
	UPROPERTY()
	TArray<FString> ProcessingQueue;

protected:
	virtual void BeginPlay() override;

private:

	// Helper to get level name for a room
	FName GetLevelNameForRoom(const FString& RoomID);

	UPROPERTY()
	TMap<ULevelStreaming*, FRoomLoadingInfo> PendingRoomLoads;

	// Reference to game state subsystem
	UPROPERTY()
	TObjectPtr<UDkGameStateSubsystem> GameStateSubsystem;

	// Handler for game state changes
	UFUNCTION()
	void OnGameStateChanged(EDkGameState NewState, EDkGameState OldState);

	// Handle for cleanup timer
	FTimerHandle CleanupTimerHandle;

	// Helper function to clear all internal data structures
	void ClearInternalState();

	// Helper function to check unload progress
	UFUNCTION()
	void CheckUnloadProgress();

	bool bIsCastleSpawning = false;
	
};

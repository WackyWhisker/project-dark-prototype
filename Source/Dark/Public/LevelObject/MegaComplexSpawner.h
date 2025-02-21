#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Data/RoomData.h"
#include "MegaComplexSpawner.generated.h"

class URoomData;
class ARoomSocketActor;

// Structure to store information about a loaded room
USTRUCT(BlueprintType)
struct FLoadedRoomInfo
{
    GENERATED_BODY()

    // Room identifier
    UPROPERTY()
    FString RoomID;

    // Reference to the level asset
    UPROPERTY()
    TSoftObjectPtr<UWorld> LevelReference;

    // Type of room
    UPROPERTY()
    ERoomType RoomType;
};

// Structure to track pending room connections
USTRUCT()
struct FPendingRoomConnection
{
    GENERATED_BODY()

    // Room being connected
    UPROPERTY()
    FString RoomID;

    // Room to connect to
    UPROPERTY()
    FString ConnectedToRoomID;

    // Socket on the source room
    UPROPERTY()
    ERoomSocketType ConnectFromSocket;

    // Socket on the target room
    UPROPERTY()
    ERoomSocketType ConnectToSocket;

    FPendingRoomConnection() {}
    FPendingRoomConnection(const FString& InRoomID, const FString& InConnectedToRoomID, 
        ERoomSocketType InConnectFromSocket, ERoomSocketType InConnectToSocket)
        : RoomID(InRoomID)
        , ConnectedToRoomID(InConnectedToRoomID)
        , ConnectFromSocket(InConnectFromSocket)
        , ConnectToSocket(InConnectToSocket)
    {}
};

// Structure to track room loading state
USTRUCT()
struct FRoomLoadingInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString RoomID;

    UPROPERTY()
    TSoftObjectPtr<UWorld> LevelReference;

    UPROPERTY()
    ERoomType RoomType;

    UPROPERTY()
    FString ConnectedToRoomID;

    UPROPERTY()
    ERoomSocketType ConnectFromSocket;

    UPROPERTY()
    ERoomSocketType ConnectToSocket;

    FRoomLoadingInfo()
        : RoomType(ERoomType::None)
    {}
};

UCLASS(Blueprintable)
class DARK_API AMegaComplexSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMegaComplexSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // Data asset containing room configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mega Complex|Setup")
    TObjectPtr<URoomData> RoomData;

    // Main spawning functions
    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Spawning")
    void SpawnCompleteMegaComplex();

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Spawning")
    void LoadEntryRoom();

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Spawning")
    void LoadRandomConnectionRoom(const FString& ConnectedToRoomID);

    // Room management functions
    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Rooms")
    void LoadRoom(const FString& RoomID, const FString& ConnectedToRoomID = "", 
        ERoomSocketType ConnectFromSocket = ERoomSocketType::Socket_Entry, 
        ERoomSocketType ConnectToSocket = ERoomSocketType::Socket_Entry);

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Rooms")
    void UnloadRoom(const FString& RoomID);

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Rooms")
    bool IsRoomLoaded(const FString& RoomID) const;

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Rooms")
    TArray<FString> GetLoadedRoomIDs() const;

    // Debug functions
    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Debug")
    void DebugDrawSockets();

    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Debug")
    void LogRoomConnections();

    // Cleanup
    UFUNCTION(BlueprintCallable, Category = "Mega Complex|Cleanup")
    void ClearCompleteMegaComplex();

protected:
    // Room loading callbacks
    UFUNCTION()
    void OnRoomLoaded();

    UFUNCTION()
    void OnRoomUnloaded();

    // Helper functions
    FString FindEntryRoomID() const;
    TArray<FString> GetNextRooms(const FString& CurrentRoomID) const;
    FTransform CalculateRoomTransform(const FString& SourceRoomID, const FString& TargetRoomID, 
        ERoomSocketType SourceSocket, ERoomSocketType TargetSocket);
    ULevelStreaming* GetLevelByReference(const TSoftObjectPtr<UWorld>& LevelReference) const;
    bool AreAnyRoomsUnloading() const;

protected:
    // Tracking loaded rooms
    UPROPERTY()
    TMap<FString, FLoadedRoomInfo> LoadedRooms;

    // Queue of rooms to process
    UPROPERTY()
    TArray<FString> ProcessingQueue;

    // Pending room connections
    UPROPERTY()
    TArray<FPendingRoomConnection> PendingConnections;

    // Currently loading rooms
    UPROPERTY()
    TMap<ULevelStreaming*, FRoomLoadingInfo> PendingRoomLoads;

    // State flags
    bool bIsSpawning;
};
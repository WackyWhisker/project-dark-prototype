// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Level.h"
#include "RoomData.generated.h"

// Define the room types we support
UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Entry       UMETA(DisplayName = "Entry Room"),
    Connection  UMETA(DisplayName = "Connection Room"),
    Exit        UMETA(DisplayName = "Exit Room"),
    None        UMETA(DisplayName = "None")
};

// Define socket types
UENUM(BlueprintType)
enum class ERoomSocketType : uint8
{
    Socket_Entry  UMETA(DisplayName = "Entry Socket"),
    Socket_Exit   UMETA(DisplayName = "Exit Socket")
};

// Structure to represent a room in our layout
USTRUCT(BlueprintType)
struct FRoom
{
    GENERATED_BODY()

    FRoom() : RoomType(ERoomType::None) {}

    // The type of room (entry, connection, exit)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
    ERoomType RoomType;

    // Array of socket types this room has
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
    TArray<ERoomSocketType> ConnectionPoints;

    // Array of level variants that can be used for this room
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room", Meta = (AllowedClasses = "World"))
    TArray<TSoftObjectPtr<UWorld>> LevelVariants;
};

// Structure to represent a connection between rooms
USTRUCT(BlueprintType)
struct FRoomConnection
{
    GENERATED_BODY()

    FRoomConnection() {}

    // Source room ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
    FString SourceRoom;

    // Target room ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
    FString TargetRoom;
};

UCLASS(BlueprintType)
class DARK_API URoomData : public UDataAsset
{
    GENERATED_BODY()

public:
    URoomData();

    // Map of room IDs to room data
    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TMap<FString, FRoom> Rooms;

    // Array of room connections
    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<FRoomConnection> Connections;

    // Get a random level variant for a room
    UFUNCTION(BlueprintPure, Category = "Rooms")
    TSoftObjectPtr<UWorld> GetRandomLevelVariantForRoom(const FString& RoomID) const;

    // Get room type for a room ID
    UFUNCTION(BlueprintPure, Category = "Rooms")
    ERoomType GetRoomType(const FString& RoomID) const;

    // Get all connection points for a room
    UFUNCTION(BlueprintPure, Category = "Rooms")
    TArray<ERoomSocketType> GetRoomConnectionPoints(const FString& RoomID) const;

    // Get all rooms of a specific type
    UFUNCTION(BlueprintPure, Category = "Rooms")
    TArray<FString> GetRoomIDsOfType(ERoomType Type) const;

    // Get connected room IDs
    UFUNCTION(BlueprintPure, Category = "Rooms")
    TArray<FString> GetConnectedRooms(const FString& RoomID) const;
};
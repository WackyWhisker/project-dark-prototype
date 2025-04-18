﻿// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CastleRoomData.generated.h"

// Define the room types we support
UENUM(BlueprintType)
enum class ECastleRoomType : uint8
{
    Entry       UMETA(DisplayName = "Entry Room"),
    Connection  UMETA(DisplayName = "Connection Room"),
    Exit        UMETA(DisplayName = "Exit Room"),
    None        UMETA(DisplayName = "None")
};

// Structure to represent a room in our layout
USTRUCT(BlueprintType)
struct FCastleRoom
{
    GENERATED_BODY()

    FCastleRoom() : RoomType(ECastleRoomType::None) {}

    // The type of room (entry, connection, exit)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Room")
    ECastleRoomType RoomType;

    // Array of socket names this room has
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Room")
    TArray<FName> ConnectionPoints;

    // Array of level names that can be used for this room
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Room")
    TArray<FName> LevelVariants;
};

// Structure to represent a connection between rooms
USTRUCT(BlueprintType)
struct FCastleRoomConnection
{
    GENERATED_BODY()

    FCastleRoomConnection() {}

    // Source room ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Room")
    FString SourceRoom;

    // Target room ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Room")
    FString TargetRoom;
};

UCLASS(BlueprintType)
class DARK_API UCastleRoomData : public UDataAsset
{
    GENERATED_BODY()

public:
    UCastleRoomData();

    // Demo switch for secret variant
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle Demo")
    bool bUseSecretVariant;



    // Map of room IDs to room data
    UPROPERTY(EditDefaultsOnly, Category = "Castle Rooms")
    TMap<FString, FCastleRoom> Rooms;

    // Array of room connections
    UPROPERTY(EditDefaultsOnly, Category = "Castle Rooms")
    TArray<FCastleRoomConnection> Connections;

    // Get a random level variant for a room
    UFUNCTION(BlueprintPure, Category = "Castle Rooms")
    FName GetRandomLevelVariantForRoom(const FString& RoomID);

    // Get room type for a room ID
    UFUNCTION(BlueprintPure, Category = "Castle Rooms")
    ECastleRoomType GetRoomType(const FString& RoomID) const;

    // Get all connection points for a room
    UFUNCTION(BlueprintPure, Category = "Castle Rooms")
    TArray<FName> GetRoomConnectionPoints(const FString& RoomID) const;

    // Get all rooms of a specific type
    UFUNCTION(BlueprintPure, Category = "Castle Rooms")
    TArray<FString> GetRoomIDsOfType(ECastleRoomType Type) const;

    // Get connected room IDs
    UFUNCTION(BlueprintPure, Category = "Castle Rooms")
    TArray<FString> GetConnectedRooms(const FString& RoomID) const;

private:
    int32 VariantCounter;
};
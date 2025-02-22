// Copyright @ Christian Reichel

#include "Data/CastleRoomData.h"

UCastleRoomData::UCastleRoomData()
{
    // Constructor
}

FName UCastleRoomData::GetRandomLevelVariantForRoom(const FString& RoomID) const
{
    if (const FCastleRoom* Room = Rooms.Find(RoomID))
    {
        if (Room->LevelVariants.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, Room->LevelVariants.Num() - 1);
            return Room->LevelVariants[RandomIndex];
        }
    }
    return NAME_None;
}

ECastleRoomType UCastleRoomData::GetRoomType(const FString& RoomID) const
{
    if (const FCastleRoom* Room = Rooms.Find(RoomID))
    {
        return Room->RoomType;
    }
    return ECastleRoomType::None;
}

TArray<FName> UCastleRoomData::GetRoomConnectionPoints(const FString& RoomID) const
{
    if (const FCastleRoom* Room = Rooms.Find(RoomID))
    {
        return Room->ConnectionPoints;
    }
    return TArray<FName>();
}

TArray<FString> UCastleRoomData::GetRoomIDsOfType(ECastleRoomType Type) const
{
    TArray<FString> Result;
    for (const auto& Pair : Rooms)
    {
        if (Pair.Value.RoomType == Type)
        {
            Result.Add(Pair.Key);
        }
    }
    return Result;
}

TArray<FString> UCastleRoomData::GetConnectedRooms(const FString& RoomID) const
{
    TArray<FString> Result;
    for (const auto& Connection : Connections)
    {
        if (Connection.SourceRoom == RoomID)
        {
            Result.Add(Connection.TargetRoom);
        }
        else if (Connection.TargetRoom == RoomID)
        {
            Result.Add(Connection.SourceRoom);
        }
    }
    return Result;
}
// Copyright @ Christian Reichel

#include "Data/CastleRoomData.h"

UCastleRoomData::UCastleRoomData()
{
    // Constructor
}

FName UCastleRoomData::GetRandomLevelVariantForRoom(const FString& RoomID)
{
    if (const FCastleRoom* Room = Rooms.Find(RoomID))
    {
        const int32 NumVariants = Room->LevelVariants.Num();
        if (NumVariants > 0)
        {
            // If only one variant, just return it
            if (NumVariants == 1)
            {
                return Room->LevelVariants[0];
            }

            int32 SelectedIndex;
            
            if (bUseSecretVariant && Room->LevelVariants.IsValidIndex(3))
            {
                SelectedIndex = 3;
            }
            else
            {
                SelectedIndex = VariantCounter % 3;
                VariantCounter = (VariantCounter + 1) % 3; // Ensures we loop 0,1,2,0,1,2...
            }

            if (Room->LevelVariants.IsValidIndex(SelectedIndex))
            {
                return Room->LevelVariants[SelectedIndex];
            }
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
// Copyright @ Christian Reichel

#include "Data/RoomData.h"

URoomData::URoomData()
{
	// Constructor
}

TSoftObjectPtr<UWorld> URoomData::GetRandomLevelVariantForRoom(const FString& RoomID) const
{
	if (const FRoom* Room = Rooms.Find(RoomID))
	{
		if (Room->LevelVariants.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, Room->LevelVariants.Num() - 1);
			return Room->LevelVariants[RandomIndex];
		}
	}
    
	// Consider logging more info here
	UE_LOG(LogTemp, Warning, TEXT("GetRandomLevelVariantForRoom: No variants found for room %s"), *RoomID);
	return TSoftObjectPtr<UWorld>();
}

ERoomType URoomData::GetRoomType(const FString& RoomID) const
{
	if (const FRoom* Room = Rooms.Find(RoomID))
	{
		return Room->RoomType;
	}
	return ERoomType::None;
}

TArray<ERoomSocketType> URoomData::GetRoomConnectionPoints(const FString& RoomID) const
{
	if (const FRoom* Room = Rooms.Find(RoomID))
	{
		return Room->ConnectionPoints;
	}
	return TArray<ERoomSocketType>();
}

TArray<FString> URoomData::GetRoomIDsOfType(ERoomType Type) const
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

TArray<FString> URoomData::GetConnectedRooms(const FString& RoomID) const
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
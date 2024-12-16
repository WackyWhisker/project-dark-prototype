// Copyright @ Christian Reichel

#include "Data/CastleRoomData.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

UCastleRoomData::UCastleRoomData()
{
    // Constructor
}

bool UCastleRoomData::LoadFromContentJSON(const FString& RelativeJSONPath)
{
    FString FullPath = FPaths::ProjectContentDir() + RelativeJSONPath;
    return LoadFromJSON(FullPath);
}

bool UCastleRoomData::LoadFromConfigJSON(const FString& RelativeJSONPath)
{
    FString FullPath = FPaths::ProjectConfigDir() + RelativeJSONPath;
    return LoadFromJSON(FullPath);
}

bool UCastleRoomData::LoadFromJSON(const FString& JSONFilePath)
{
    // Clear existing data
    Rooms.Empty();
    Connections.Empty();

    // Read JSON file
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *JSONFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file: %s"), *JSONFilePath);
        return false;
    }

    // Parse JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data from: %s"), *JSONFilePath);
        return false;
    }

    // Parse nodes
    const TSharedPtr<FJsonObject>* NodesObject;
    if (JsonObject->TryGetObjectField(TEXT("nodes"), NodesObject))
    {
        for (const auto& NodePair : (*NodesObject)->Values)
        {
            FCastleRoom Room;
            const TSharedPtr<FJsonObject>& NodeData = NodePair.Value->AsObject();

            // Parse room type
            FString RoomTypeStr = NodeData->GetStringField(TEXT("roomType"));
            if (RoomTypeStr == "entry") Room.RoomType = ECastleRoomType::Entry;
            else if (RoomTypeStr == "connection") Room.RoomType = ECastleRoomType::Connection;
            else if (RoomTypeStr == "exit") Room.RoomType = ECastleRoomType::Exit;
            else Room.RoomType = ECastleRoomType::None;

            // Parse connection points
            const TArray<TSharedPtr<FJsonValue>>* ConnectionPoints;
            if (NodeData->TryGetArrayField(TEXT("connectionPoints"), ConnectionPoints))
            {
                for (const auto& Point : *ConnectionPoints)
                {
                    Room.ConnectionPoints.Add(FName(*Point->AsString()));
                }
            }

            // Parse level variants
            const TArray<TSharedPtr<FJsonValue>>* LevelVariants;
            if (NodeData->TryGetArrayField(TEXT("levelVariants"), LevelVariants))
            {
                for (const auto& Variant : *LevelVariants)
                {
                    Room.LevelVariants.Add(ConvertPathToLevelName(Variant->AsString()));
                }
            }

            // Add room to map
            Rooms.Add(NodePair.Key, Room);
        }
    }

    // Parse edges
    const TArray<TSharedPtr<FJsonValue>>* EdgesArray;
    if (JsonObject->TryGetArrayField(TEXT("edges"), EdgesArray))
    {
        for (const auto& Edge : *EdgesArray)
        {
            FCastleRoomConnection Connection;
            const TSharedPtr<FJsonObject>& EdgeObject = Edge->AsObject();
            
            Connection.SourceRoom = EdgeObject->GetStringField(TEXT("source"));
            Connection.TargetRoom = EdgeObject->GetStringField(TEXT("target"));
            
            Connections.Add(Connection);
        }
    }

    return true;
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

FName UCastleRoomData::ConvertPathToLevelName(const FString& Path)
{
    // Remove "/Game/Levels/CastleRooms/" prefix and any file extension
    FString LevelName = Path;
    LevelName.RemoveFromStart(TEXT("/Game/Levels/CastleRooms/"));
    
    // Remove any extension if present
    FString BaseFileName = FPaths::GetBaseFilename(LevelName);
    
    return FName(*BaseFileName);
}
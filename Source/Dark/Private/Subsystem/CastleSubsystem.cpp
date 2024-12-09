// Copyright @ Christian Reichel

#include "Subsystem/CastleSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Data/CastleRoomData.h"

DEFINE_LOG_CATEGORY(CastleLog)

void UCastleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadedRooms.Empty();

	if (!RoomData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Castle Room Data Asset configured in project settings!"));
	}
}

 
void UCastleSubsystem::LoadRandomRoom()
{
	if (RoomData && RoomData->AvailableRooms.Num() > 0)
	{
		// Get random index
		int32 RandomIndex = FMath::RandRange(0, RoomData->AvailableRooms.Num() - 1);
		FName RandomRoom = RoomData->AvailableRooms[RandomIndex];
        
		// Load the random room
		LoadRoom(RandomRoom);
		UE_LOG(LogTemp, Log, TEXT("Loading random room: %s"), *RandomRoom.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No available rooms configured in Room Data Asset!"));
	}
}

void UCastleSubsystem::TestSubsystemAccess()
{
	UE_LOG(CastleLog, Warning, TEXT("Castle Subsystem accessed successfully"));
}

void UCastleSubsystem::LoadRoom(FName RoomLevelName)
{
	if (UWorld* World = GetWorld())
	{
		// Check if already loaded
		if (!LoadedRooms.Contains(RoomLevelName))
		{
			// Create latent action info for the loading callback
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.ExecutionFunction = "OnRoomLoaded";
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = FMath::Rand();  // Unique ID for this operation

			// Start loading the level
			UGameplayStatics::LoadStreamLevel(World, RoomLevelName, true, false, LatentInfo);
			LoadedRooms.Add(RoomLevelName);
		}
	}
}

void UCastleSubsystem::UnloadRoom(FName RoomLevelName)
{
	if (UWorld* World = GetWorld())
	{
		// Check if it's loaded
		if (LoadedRooms.Contains(RoomLevelName))
		{
			// Create latent action info for the unloading callback
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.ExecutionFunction = "OnRoomUnloaded";
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = FMath::Rand();

			// Start unloading the level
			UGameplayStatics::UnloadStreamLevel(World, RoomLevelName, LatentInfo, false);
			LoadedRooms.Remove(RoomLevelName);
		}
	}
}

bool UCastleSubsystem::IsRoomLoaded(FName RoomLevelName)
{
	if (UWorld* World = GetWorld())
	{
		if (ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(World, RoomLevelName))
		{
			return StreamingLevel->IsLevelLoaded();
		}
	}
	return false;
}

void UCastleSubsystem::OnRoomLoaded()
{
	// Room has finished loading
	UE_LOG(LogTemp, Log, TEXT("Room finished loading"));
}

void UCastleSubsystem::OnRoomUnloaded()
{
	// Room has finished unloading
	UE_LOG(LogTemp, Log, TEXT("Room finished unloading"));
}

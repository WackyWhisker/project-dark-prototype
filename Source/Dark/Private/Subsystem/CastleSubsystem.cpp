// Copyright @ Christian Reichel

#include "Subsystem/CastleSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Data/CastleRoomData.h"
#include "Engine/LevelStreamingDynamic.h"
#include "WorldSetting/CastleWorldSettings.h"

DEFINE_LOG_CATEGORY(CastleLog)

void UCastleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CastleWorldSettings = Cast<ACastleWorldSettings>(GetWorld()->GetWorldSettings());
	if (!CastleWorldSettings) return;
	
	LoadedRooms.Empty();
	
}

 
void UCastleSubsystem::LoadRandomRoom()
{
	
	if (CastleWorldSettings->RoomData && CastleWorldSettings->RoomData->AvailableRooms.Num() > 0)
	{
		// Get random index
		int32 RandomIndex = FMath::RandRange(0, CastleWorldSettings->RoomData->AvailableRooms.Num() - 1);
		FName RandomRoom = CastleWorldSettings->RoomData->AvailableRooms[RandomIndex];
        
		// Load the random room
		LoadRoom(RandomRoom);
		UE_LOG(CastleLog, Log, TEXT("Loading random room: %s"), *RandomRoom.ToString());
	}
	else
	{
		UE_LOG(CastleLog, Warning, TEXT("No available rooms configured in Room Data Asset!"));
	}
}

void UCastleSubsystem::TestSubsystemAccess()
{
	UE_LOG(CastleLog, Warning, TEXT("Castle Subsystem accessed successfully"));
}

void UCastleSubsystem::LoadRoom(FName RoomLevelName) {
	if (UWorld* TargetWorld = GetWorld()) {
		if (!LoadedRooms.Contains(RoomLevelName)) {
			bool bSucceeded = false;
			ULevelStreamingDynamic* NewLevel = ULevelStreamingDynamic::LoadLevelInstance(TargetWorld, RoomLevelName.ToString(), FVector::ZeroVector, FRotator::ZeroRotator, /*out*/ bSucceeded);

			if (bSucceeded) {
				LoadedRooms.Add(RoomLevelName);
				UE_LOG(LogTemp, Log, TEXT("Successfully created level instance for room: %s"), *RoomLevelName.ToString());
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to create level instance for room: %s"), *RoomLevelName.ToString());
			}
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
	UE_LOG(CastleLog, Log, TEXT("Room finished loading"));
}

void UCastleSubsystem::OnRoomUnloaded()
{
	// Room has finished unloading
	UE_LOG(CastleLog, Log, TEXT("Room finished unloading"));
}

// Copyright @ Christian Reichel


#include "Managers/BiomeSectionsManager.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "WorldSetting/CastleWorldSettings.h"
#include "Subsystem/CastleSocketActor.h"
#include "Subsystem/DkGameStateSubsystem.h"


// Sets default values
ABiomeSectionsManager::ABiomeSectionsManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABiomeSectionsManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (UWorld* World = GetWorld())
	{
		GameStateSubsystem = World->GetSubsystem<UDkGameStateSubsystem>();
		if (GameStateSubsystem)
		{
			GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &ABiomeSectionsManager::OnGameStateChanged);
		}
	}

	LoadedRooms.Empty();
	ProcessingQueue.Empty();
	
}

void ABiomeSectionsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABiomeSectionsManager::SpawnCompleteCastle()
{
	if (bIsCastleSpawning) return;
    
	if (!RoomData)
	{
		return;
	}

	bIsCastleSpawning = true;

	LoadedRooms.Empty();
	ProcessingQueue.Empty();

	FString EntryRoomID = FindEntryRoomID();
	if (EntryRoomID.IsEmpty())
	{
		return;
	}

	TArray<FString> ProcessedRooms;
	ProcessingQueue.Add(EntryRoomID);

	while (ProcessingQueue.Num() > 0)
	{
		FString CurrentRoomID = ProcessingQueue[0];
        
		if (!ProcessedRooms.Contains(CurrentRoomID))
		{
			ProcessedRooms.Add(CurrentRoomID);
            
			TArray<FString> NextRooms = GetNextRooms(CurrentRoomID);
			for (const FString& NextRoomID : NextRooms)
			{
				if (!ProcessedRooms.Contains(NextRoomID))
				{
					ProcessingQueue.Add(NextRoomID);
				}
			}
		}
        
		ProcessingQueue.RemoveAt(0);
	}

	ProcessingQueue = ProcessedRooms;

	LoadRoom(ProcessingQueue[0], "", NAME_None, NAME_None);
}

void ABiomeSectionsManager::ProcessNextRoom()
{
	if (ProcessingQueue.Num() > 0)
	{
		FString RoomToLoad = ProcessingQueue[0];
		ProcessingQueue.RemoveAt(0);

		FString ConnectToRoomID;
		FName SourceSocket = NAME_None;
		FName TargetSocket = NAME_None;

		if (RoomData->GetRoomType(RoomToLoad) != ECastleRoomType::Entry)
		{
			for (const auto& LoadedRoom : LoadedRooms)
			{
				TArray<FString> ConnectedRooms = RoomData->GetConnectedRooms(LoadedRoom.Key);
				if (ConnectedRooms.Contains(RoomToLoad))
				{
					ConnectToRoomID = LoadedRoom.Key;
                    
					TArray<FName> SourcePoints = RoomData->GetRoomConnectionPoints(ConnectToRoomID);
					TArray<FName> TargetPoints = RoomData->GetRoomConnectionPoints(RoomToLoad);
                    
					if (SourcePoints.Num() > 0 && TargetPoints.Num() > 0)
					{
						SourceSocket = SourcePoints[0];
						TargetSocket = TargetPoints[0];
					}
                    
					break;
				}
			}
		}

		LoadRoom(RoomToLoad, ConnectToRoomID, SourceSocket, TargetSocket);
	}
}

void ABiomeSectionsManager::LoadEntryRoom()
{
	if (!RoomData) return;

	TArray<FString> EntryRooms = RoomData->GetRoomIDsOfType(ECastleRoomType::Entry);
	if (EntryRooms.Num() > 0)
	{
		LoadRoom(EntryRooms[0]);
	}
}

void ABiomeSectionsManager::LoadRandomConnectionRoom(const FString& ConnectedToRoomID)
{
	if (!RoomData) return;

	TArray<FString> ConnectionRooms = RoomData->GetRoomIDsOfType(ECastleRoomType::Connection);
	if (ConnectionRooms.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, ConnectionRooms.Num() - 1);
		LoadRoom(ConnectionRooms[RandomIndex]);
	}
}

void ABiomeSectionsManager::LoadRoom(const FString& RoomID, const FString& ConnectedToRoomID, FName ConnectFromSocket,
	FName ConnectToSocket)
{
	if (!RoomData) return;
    
    if (UWorld* World = GetWorld())
    {
        FString LocalRoomID = RoomID;
        FString LocalConnectedToRoomID = ConnectedToRoomID;
        FName LocalConnectFromSocket = ConnectFromSocket;
        FName LocalConnectToSocket = ConnectToSocket;

        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, [this, LocalRoomID, LocalConnectedToRoomID, 
            LocalConnectFromSocket, LocalConnectToSocket]()
        {
            if (!LoadedRooms.Contains(LocalRoomID))
            {
                FName LevelName = GetLevelNameForRoom(LocalRoomID);
                if (LevelName == NAME_None) return;

            	FVector SpawnPosition = GetActorLocation();
				FRotator SpawnRotation = GetActorRotation();

                if (!LocalConnectedToRoomID.IsEmpty())
                {
                    FLoadedRoomInfo* ConnectedRoom = LoadedRooms.Find(LocalConnectedToRoomID);
                    if (ConnectedRoom)
                    {
                        ULevelStreaming* ConnectedLevel = GetLevelByName(ConnectedRoom->LevelName);
                        if (ConnectedLevel && ConnectedLevel->GetLoadedLevel())
                        {
                            bool bFoundSocket = false;
                            for (AActor* Actor : ConnectedLevel->GetLoadedLevel()->Actors)
                            {
                                if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
                                {
                                    if (SocketActor->SocketName == LocalConnectFromSocket)
                                    {
                                        SpawnPosition = SocketActor->GetActorLocation();
                                        SpawnRotation = SocketActor->GetActorRotation() + FRotator(0, 0.0f, 0);
            
                                        bFoundSocket = true;
                                        break;
                                    }
                                }
                            }
                            
                            if (!bFoundSocket)
                            {
                                return;
                            }
                        }
                        else
                        {
                            return;
                        }
                    }
                    else
                    {
                        return;
                    }
                }

                bool bSucceeded = false;
                ULevelStreamingDynamic* NewLevel = ULevelStreamingDynamic::LoadLevelInstance(
                    GetWorld(),
                    LevelName.ToString(),
                    SpawnPosition,
                    SpawnRotation,
                    bSucceeded
                );

                if (bSucceeded)
                {
                    FLoadedRoomInfo RoomInfo;
                    RoomInfo.RoomID = LocalRoomID;
                    RoomInfo.LevelName = LevelName;
                    RoomInfo.RoomType = RoomData->GetRoomType(LocalRoomID);
                    LoadedRooms.Add(LocalRoomID, RoomInfo);

                    NewLevel->OnLevelLoaded.AddDynamic(this, &ABiomeSectionsManager::OnRoomLoaded);
                }
            }
        }, 0.25f, false);
    }
}

void ABiomeSectionsManager::UnloadRoom(const FString& RoomID)
{
	if (UWorld* World = GetWorld())
	{
		if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
		{
			if (ULevelStreaming* Level = GetLevelByName(RoomInfo->LevelName))
			{
				FString PackageName = Level->GetWorldAssetPackageName();
                
				FLatentActionInfo LatentInfo;
				LatentInfo.CallbackTarget = this;
				LatentInfo.ExecutionFunction = "OnRoomUnloaded";
				LatentInfo.Linkage = 0;
				LatentInfo.UUID = FMath::Rand();

				Level->OnLevelUnloaded.AddDynamic(this, &ABiomeSectionsManager::OnRoomUnloaded);
				UGameplayStatics::UnloadStreamLevel(World, FName(*PackageName), LatentInfo, false);
			}
		}
	}
}

bool ABiomeSectionsManager::IsRoomLoaded(const FString& RoomID) const
{
	return LoadedRooms.Contains(RoomID);
}

TArray<FString> ABiomeSectionsManager::GetLoadedRoomIDs() const
{
	TArray<FString> RoomIDs;
	LoadedRooms.GetKeys(RoomIDs);
	return RoomIDs;
}

void ABiomeSectionsManager::ProcessPendingConnections()
{
	for (int32 i = PendingConnections.Num() - 1; i >= 0; --i)
	{
		const FPendingRoomConnection& Connection = PendingConnections[i];
        
		if (IsRoomLoaded(Connection.RoomID) && IsRoomLoaded(Connection.ConnectedToRoomID))
		{
			FTransform NewTransform = CalculateRoomTransform(
				Connection.ConnectedToRoomID, 
				Connection.RoomID,
				Connection.ConnectFromSocket, 
				Connection.ConnectToSocket);

			if (ULevelStreaming* Level = GetLevelByName(
				LoadedRooms.Find(Connection.RoomID)->LevelName))
			{
				Level->LevelTransform = NewTransform;
				PendingConnections.RemoveAt(i);
                
				if (UWorld* World = GetWorld())
				{
					World->UpdateLevelStreaming();
				}
			}
		}
	}
}

void ABiomeSectionsManager::ClearCompleteCastle()
{
	if (UWorld* World = GetWorld())
	{
		TArray<ULevelStreaming*> LevelsToRemove = World->GetStreamingLevels();
        
		TArray<FString> RoomIDsToUnload = GetLoadedRoomIDs();
        
		if (RoomIDsToUnload.Num() > 0)
		{
			for (const FString& RoomID : RoomIDsToUnload)
			{
				UnloadRoom(RoomID);
			}
            
			World->GetTimerManager().ClearTimer(CleanupTimerHandle);
            
			World->GetTimerManager().SetTimer(CleanupTimerHandle, 
				FTimerDelegate::CreateLambda([this, World, LevelsToRemove]()
				{
					if (!AreAnyRoomsUnloading())
					{
						for (ULevelStreaming* Level : LevelsToRemove)
						{
							if (Level)
							{
								World->RemoveStreamingLevel(Level);
							}
						}
                        
						GEngine->ForceGarbageCollection(true);
                        
						ClearInternalState();
                        
						World->GetTimerManager().ClearTimer(CleanupTimerHandle);
					}
				}), 0.1f, true);
		}
		else
		{
			ClearInternalState();
		}
	}
}

void ABiomeSectionsManager::OnRoomLoaded()
{
	ProcessingQueue.RemoveAt(0);
    
	if (ProcessingQueue.Num() > 0)
	{
		FString NextRoomID = ProcessingQueue[0];
		FString ConnectToRoomID;
		FName SourceSocket = NAME_None;
		FName TargetSocket = NAME_None;

		for (const auto& LoadedRoom : LoadedRooms)
		{
			TArray<FString> ConnectedRooms = RoomData->GetConnectedRooms(LoadedRoom.Key);
			if (ConnectedRooms.Contains(NextRoomID))
			{
				ConnectToRoomID = LoadedRoom.Key;
                
				TArray<FName> SourcePoints = RoomData->GetRoomConnectionPoints(ConnectToRoomID);
				TArray<FName> TargetPoints = RoomData->GetRoomConnectionPoints(NextRoomID);
                
				if (SourcePoints.Num() > 0 && TargetPoints.Num() > 0)
				{
					SourceSocket = SourcePoints.Num() > 1 ? SourcePoints.Last() : SourcePoints[0];
					TargetSocket = TargetPoints[0];
				}
                
				break;
			}
		}

		LoadRoom(NextRoomID, ConnectToRoomID, SourceSocket, TargetSocket);
	}
}

void ABiomeSectionsManager::OnRoomUnloaded()
{
	if (UWorld* World = GetWorld())
	{
		TArray<FString> RoomIDsToUnload = GetLoadedRoomIDs();
        
		for (auto It = LoadedRooms.CreateIterator(); It; ++It)
		{
			if (ULevelStreaming* Level = GetLevelByName(It->Value.LevelName))
			{
				if (!Level->IsLevelLoaded())
				{
					LoadedRooms.Remove(It->Key);
					break;
				}
			}
		}
	}
}

FString ABiomeSectionsManager::FindEntryRoomID() const
{
	if (!RoomData)
	{
		return FString();
	}

	TArray<FString> EntryRooms = RoomData->GetRoomIDsOfType(ECastleRoomType::Entry);
	if (EntryRooms.Num() > 0)
	{
		return EntryRooms[0];
	}

	return FString();
}

TArray<FString> ABiomeSectionsManager::GetNextRooms(const FString& CurrentRoomID) const
{
	TArray<FString> NextRooms;

	if (!RoomData)
	{
		return NextRooms;
	}

	for (const FCastleRoomConnection& Connection : RoomData->Connections)
	{
		if (Connection.SourceRoom == CurrentRoomID)
		{
			NextRooms.Add(Connection.TargetRoom);
		}
	}

	return NextRooms;
}

FTransform ABiomeSectionsManager::CalculateRoomTransform(const FString& SourceRoomID, const FString& TargetRoomID,
	FName SourceSocket, FName TargetSocket)
{
	FLoadedRoomInfo* SourceInfo = LoadedRooms.Find(SourceRoomID);
    if (!SourceInfo)
    {
        return FTransform::Identity;
    }

    ULevelStreaming* SourceLevel = GetLevelByName(SourceInfo->LevelName);
    if (!SourceLevel || !SourceLevel->GetLoadedLevel())
    {
        return FTransform::Identity;
    }

    ACastleSocketActor* SourceSocketActor = nullptr;
    for (AActor* Actor : SourceLevel->GetLoadedLevel()->Actors)
    {
        if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
        {
            if (SocketActor->SocketName == SourceSocket)
            {
                SourceSocketActor = SocketActor;
                break;
            }
        }
    }

    if (!SourceSocketActor)
    {
        return FTransform::Identity;
    }

    ULevelStreaming* TargetLevel = nullptr;
    ACastleSocketActor* TargetSocketActor = nullptr;
    
    for (const auto& LoadedRoom : LoadedRooms)
    {
        if (LoadedRoom.Key == TargetRoomID)
        {
            TargetLevel = GetLevelByName(LoadedRoom.Value.LevelName);
            break;
        }
    }

    if (TargetLevel && TargetLevel->GetLoadedLevel())
    {
        for (AActor* Actor : TargetLevel->GetLoadedLevel()->Actors)
        {
            if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
            {
                if (SocketActor->SocketName == TargetSocket)
                {
                    TargetSocketActor = SocketActor;
                    break;
                }
            }
        }
    }

    if (!TargetSocketActor)
    {
        return FTransform::Identity;
    }

    FTransform ResultTransform;

    FVector SourceSocketWorldPos = SourceSocketActor->GetActorLocation();
    
    FVector TargetSocketLocalPos = TargetSocketActor->GetActorLocation() - TargetLevel->LevelTransform.GetLocation();

    FVector RequiredLevelPosition = SourceSocketWorldPos - TargetSocketLocalPos;
    
    ResultTransform.SetLocation(RequiredLevelPosition);
    ResultTransform.SetRotation(FRotator(0, 180.0f, 0).Quaternion());

    return ResultTransform;
}

ULevelStreaming* ABiomeSectionsManager::GetLevelByName(const FName& LevelName) const
{
	if (UWorld* World = GetWorld())
	{
		TArray<ULevelStreaming*> StreamingLevels = World->GetStreamingLevels();
        
		for (ULevelStreaming* Level : StreamingLevels)
		{
			if (Level)
			{
				FString PackageName = Level->GetWorldAssetPackageName();
                
				if (PackageName.Contains(LevelName.ToString()))
				{
					return Level;
				}
			}
		}
	}
	return nullptr;
}

bool ABiomeSectionsManager::AreAnyRoomsUnloading() const
{
	if (UWorld* World = GetWorld())
	{
		for (ULevelStreaming* Level : World->GetStreamingLevels())
		{
			if (Level && Level->IsLevelLoaded() && Level->IsStreamingStatePending())
			{
				return true;
			}
		}
	}
	return false;
}

FName ABiomeSectionsManager::GetLevelNameForRoom(const FString& RoomID)
{
	if (!RoomData) return NAME_None;

	return RoomData->GetRandomLevelVariantForRoom(RoomID);
}

void ABiomeSectionsManager::OnGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
	if (NewState == EDkGameState::Resetting)
	{
		if (GameStateSubsystem)
		{
			GameStateSubsystem->RegisterForReset(this);
		}
        
		ClearCompleteCastle();
	}
}

void ABiomeSectionsManager::ClearInternalState()
{
	LoadedRooms.Empty();
	ProcessingQueue.Empty();
	PendingConnections.Empty();
	PendingRoomLoads.Empty();
	bIsCastleSpawning = false;
    
	if (GameStateSubsystem)
	{
		GameStateSubsystem->NotifyResetComplete(this);
	}
}

void ABiomeSectionsManager::CheckUnloadProgress()
{
	if (!AreAnyRoomsUnloading())
	{
		ClearInternalState();
        
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CleanupTimerHandle);
		}
	}
}


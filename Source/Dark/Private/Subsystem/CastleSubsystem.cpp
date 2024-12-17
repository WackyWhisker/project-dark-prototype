// Copyright @ Christian Reichel

#include "Subsystem/CastleSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Data/CastleRoomData.h"
#include "Engine/LevelStreamingDynamic.h"
#include "WorldSetting/CastleWorldSettings.h"
#include "DrawDebugHelpers.h"
#include "Subsystem/CastleSocketActor.h"
#include "Subsystem/DkGameStateSubsystem.h"

DEFINE_LOG_CATEGORY(CastleLog);

void UCastleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // Initialize dependency first
    Collection.InitializeDependency<UDkGameStateSubsystem>();
    
    Super::Initialize(Collection);

    UE_LOG(CastleLog, Warning, TEXT("Castle Subsystem initialization starting"));

    if (UWorld* World = GetWorld())
    {
        GameStateSubsystem = World->GetSubsystem<UDkGameStateSubsystem>();
        if (GameStateSubsystem)
        {
            UE_LOG(CastleLog, Warning, TEXT("Found GameStateSubsystem, subscribing to events"));
            GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UCastleSubsystem::OnGameStateChanged);
        }
        else
        {
            UE_LOG(CastleLog, Error, TEXT("GameStateSubsystem not found during Castle initialization - This should never happen due to dependency resolution"));
        }
    }

    CastleWorldSettings = Cast<ACastleWorldSettings>(GetWorld()->GetWorldSettings());
    if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
    {
        UE_LOG(CastleLog, Warning, TEXT("Failed to initialize Castle Subsystem - Missing World Settings or Room Data"));
        return;
    }

    bool bSuccess = CastleWorldSettings->RoomData->LoadFromContentJSON("Data/dungeon_data.json");
    if (!bSuccess)
    {
        UE_LOG(CastleLog, Error, TEXT("Failed to load dungeon data from JSON"));
        return;
    }

    LoadedRooms.Empty();
    ProcessingQueue.Empty();
}

void UCastleSubsystem::TestSubsystemAccess()
{
   UE_LOG(CastleLog, Warning, TEXT("Castle Subsystem accessed successfully"));
}

void UCastleSubsystem::SpawnCompleteDungeon()
{
    if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
    {
        UE_LOG(CastleLog, Error, TEXT("Cannot spawn dungeon - Missing Room Data"));
        return;
    }

    // Clear any existing state
    LoadedRooms.Empty();
    ProcessingQueue.Empty();

    // Find entry room and start the sequence
    FString EntryRoomID = FindEntryRoomID();
    if (EntryRoomID.IsEmpty())
    {
        UE_LOG(CastleLog, Error, TEXT("No entry room found in dungeon data"));
        return;
    }

    // Build complete processing queue first
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

    // Set up processing queue with all rooms in correct order
    ProcessingQueue = ProcessedRooms;

    // Start with the entry room
    LoadRoom(ProcessingQueue[0], "", NAME_None, NAME_None);
}

void UCastleSubsystem::ProcessNextRoom()
{
    if (ProcessingQueue.Num() > 0)
    {
        FString RoomToLoad = ProcessingQueue[0];
        ProcessingQueue.RemoveAt(0);

        // Find the room to connect to (if not entry room)
        FString ConnectToRoomID;
        FName SourceSocket = NAME_None;
        FName TargetSocket = NAME_None;

        if (CastleWorldSettings->RoomData->GetRoomType(RoomToLoad) != ECastleRoomType::Entry)
        {
            // Find a loaded room that this connects to
            for (const auto& LoadedRoom : LoadedRooms)
            {
                TArray<FString> ConnectedRooms = CastleWorldSettings->RoomData->GetConnectedRooms(LoadedRoom.Key);
                if (ConnectedRooms.Contains(RoomToLoad))
                {
                    ConnectToRoomID = LoadedRoom.Key;
                    
                    // Get connection points
                    TArray<FName> SourcePoints = CastleWorldSettings->RoomData->GetRoomConnectionPoints(ConnectToRoomID);
                    TArray<FName> TargetPoints = CastleWorldSettings->RoomData->GetRoomConnectionPoints(RoomToLoad);
                    
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


void UCastleSubsystem::LoadEntryRoom()
{
   if (!CastleWorldSettings || !CastleWorldSettings->RoomData) return;

   TArray<FString> EntryRooms = CastleWorldSettings->RoomData->GetRoomIDsOfType(ECastleRoomType::Entry);
   if (EntryRooms.Num() > 0)
   {
       LoadRoom(EntryRooms[0]);
   }
}

void UCastleSubsystem::LoadRandomConnectionRoom(const FString& ConnectedToRoomID)
{
   if (!CastleWorldSettings || !CastleWorldSettings->RoomData) return;

   TArray<FString> ConnectionRooms = CastleWorldSettings->RoomData->GetRoomIDsOfType(ECastleRoomType::Connection);
   if (ConnectionRooms.Num() > 0)
   {
       int32 RandomIndex = FMath::RandRange(0, ConnectionRooms.Num() - 1);
       LoadRoom(ConnectionRooms[RandomIndex]);
   }
}

void UCastleSubsystem::LoadRoom(const FString& RoomID, const FString& ConnectedToRoomID, 
    FName ConnectFromSocket, FName ConnectToSocket)
{
    if (!CastleWorldSettings || !CastleWorldSettings->RoomData) return;
    
    if (UWorld* World = GetWorld())
    {
        // Store all parameters in local variables to use in the lambda
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

                FVector SpawnPosition = FVector::ZeroVector;
                FRotator SpawnRotation = FRotator::ZeroRotator;

                UE_LOG(CastleLog, Warning, TEXT("=== Loading Room: %s ==="), *LocalRoomID);
                UE_LOG(CastleLog, Warning, TEXT("Connected To: %s"), LocalConnectedToRoomID.IsEmpty() ? TEXT("None") : *LocalConnectedToRoomID);
                UE_LOG(CastleLog, Warning, TEXT("Connect From Socket: %s"), *LocalConnectFromSocket.ToString());
                UE_LOG(CastleLog, Warning, TEXT("Connect To Socket: %s"), *LocalConnectToSocket.ToString());

                // If this isn't the entry room, get the world position from the connected room's socket
                if (!LocalConnectedToRoomID.IsEmpty())
                {
                    FLoadedRoomInfo* ConnectedRoom = LoadedRooms.Find(LocalConnectedToRoomID);
                    if (ConnectedRoom)
                    {
                        UE_LOG(CastleLog, Warning, TEXT("Found Connected Room: %s (Level: %s)"), 
                            *ConnectedRoom->RoomID, *ConnectedRoom->LevelName.ToString());

                        ULevelStreaming* ConnectedLevel = GetLevelByName(ConnectedRoom->LevelName);
                        if (ConnectedLevel && ConnectedLevel->GetLoadedLevel())
                        {
                            UE_LOG(CastleLog, Warning, TEXT("Connected Level Transform: %s"), 
                                *ConnectedLevel->LevelTransform.ToString());

                            // Find the socket in the connected room
                            bool bFoundSocket = false;
                            for (AActor* Actor : ConnectedLevel->GetLoadedLevel()->Actors)
                            {
                                if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
                                {
                                    FVector ActorLocation = SocketActor->GetActorLocation();
                                    FTransform ActorTransform = SocketActor->GetActorTransform();
        
                                    UE_LOG(CastleLog, Warning, TEXT("Found Socket: %s"), *SocketActor->SocketName.ToString());
                                    UE_LOG(CastleLog, Warning, TEXT("  Actor Location: %s"), *ActorLocation.ToString());
                                    UE_LOG(CastleLog, Warning, TEXT("  Actor Transform: %s"), *ActorTransform.ToString());
                                    UE_LOG(CastleLog, Warning, TEXT("  Level Transform: %s"), 
                                        *ConnectedLevel->LevelTransform.ToString());

                                    if (SocketActor->SocketName == LocalConnectFromSocket)
                                    {
                                        SpawnPosition = ActorLocation;
                                        SpawnRotation = SocketActor->GetActorRotation() + FRotator(0, 0.0f, 0);
            
                                        UE_LOG(CastleLog, Warning, TEXT("Using Socket for Spawn - Position: %s, Rotation: %s"), 
                                            *SpawnPosition.ToString(), *SpawnRotation.ToString());
            
                                        bFoundSocket = true;
                                        break;
                                    }
                                }
                            }
                            
                            if (!bFoundSocket)
                            {
                                UE_LOG(CastleLog, Error, TEXT("Failed to find socket %s in room %s"), 
                                    *LocalConnectFromSocket.ToString(), *LocalConnectedToRoomID);
                            }
                        }
                        else
                        {
                            UE_LOG(CastleLog, Error, TEXT("Connected level not found or not loaded"));
                        }
                    }
                    else
                    {
                        UE_LOG(CastleLog, Error, TEXT("Connected room info not found: %s"), *LocalConnectedToRoomID);
                    }
                }
                else
                {
                    UE_LOG(CastleLog, Warning, TEXT("Entry room - spawning at origin"));
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
                    RoomInfo.RoomType = CastleWorldSettings->RoomData->GetRoomType(LocalRoomID);
                    LoadedRooms.Add(LocalRoomID, RoomInfo);

                    NewLevel->OnLevelLoaded.AddDynamic(this, &UCastleSubsystem::OnRoomLoaded);
                    
                    UE_LOG(CastleLog, Warning, TEXT("=== Room %s spawn complete ==="), *LocalRoomID);
                    UE_LOG(CastleLog, Warning, TEXT("Final Position: %s"), *SpawnPosition.ToString());
                    UE_LOG(CastleLog, Warning, TEXT("Final Rotation: %s"), *SpawnRotation.ToString());
                }
                else
                {
                    UE_LOG(CastleLog, Error, TEXT("Failed to load level instance for room: %s"), *LocalRoomID);
                }
            }
        }, 0.5f, false);
    }
}

void UCastleSubsystem::UnloadRoom(const FString& RoomID)
{
    if (UWorld* World = GetWorld())
    {
        if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
        {
            if (ULevelStreaming* Level = GetLevelByName(RoomInfo->LevelName))
            {
                FLatentActionInfo LatentInfo;
                LatentInfo.CallbackTarget = this;
                LatentInfo.ExecutionFunction = "OnRoomUnloaded";
                LatentInfo.Linkage = 0;
                LatentInfo.UUID = FMath::Rand();

                Level->OnLevelUnloaded.AddDynamic(this, &UCastleSubsystem::OnRoomUnloaded);
                UGameplayStatics::UnloadStreamLevel(World, RoomInfo->LevelName, LatentInfo, false);
            }
        }
    }
}

bool UCastleSubsystem::IsRoomLoaded(const FString& RoomID) const
{
   return LoadedRooms.Contains(RoomID);
}

TArray<FString> UCastleSubsystem::GetLoadedRoomIDs() const
{
   TArray<FString> RoomIDs;
   LoadedRooms.GetKeys(RoomIDs);
   return RoomIDs;
}

FString UCastleSubsystem::FindEntryRoomID() const
{
   if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
   {
       return FString();
   }

   TArray<FString> EntryRooms = CastleWorldSettings->RoomData->GetRoomIDsOfType(ECastleRoomType::Entry);
   if (EntryRooms.Num() > 0)
   {
       return EntryRooms[0];
   }

   return FString();
}

TArray<FString> UCastleSubsystem::GetNextRooms(const FString& CurrentRoomID) const
{
   TArray<FString> NextRooms;

   if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
   {
       return NextRooms;
   }

   for (const FCastleRoomConnection& Connection : CastleWorldSettings->RoomData->Connections)
   {
       if (Connection.SourceRoom == CurrentRoomID)
       {
           NextRooms.Add(Connection.TargetRoom);
       }
   }

   return NextRooms;
}

FTransform UCastleSubsystem::CalculateRoomTransform(const FString& SourceRoomID, 
    const FString& TargetRoomID, FName SourceSocket, FName TargetSocket)
{
    // Get source room socket
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

    // Find source socket actor
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

    // Find target socket actor in currently loading level
    ULevelStreaming* TargetLevel = nullptr;
    ACastleSocketActor* TargetSocketActor = nullptr;
    
    // Find the level that's currently loading for our target room
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

    // Calculate the transform
    FTransform ResultTransform;

    // Get the source socket's world position (considering level transform)
    FVector SourceSocketWorldPos = SourceSocketActor->GetActorLocation();
    
    // Get target socket's position relative to its level
    FVector TargetSocketLocalPos = TargetSocketActor->GetActorLocation() - TargetLevel->LevelTransform.GetLocation();

    // Calculate where to place the level so the target socket lines up with source socket
    FVector RequiredLevelPosition = SourceSocketWorldPos - TargetSocketLocalPos;
    
    // Set position and rotation
    ResultTransform.SetLocation(RequiredLevelPosition);
    ResultTransform.SetRotation(FRotator(0, 180.0f, 0).Quaternion());

    UE_LOG(CastleLog, Warning, TEXT("=== Transform Calculation ==="));
    UE_LOG(CastleLog, Warning, TEXT("Source Socket World Pos: %s"), *SourceSocketWorldPos.ToString());
    UE_LOG(CastleLog, Warning, TEXT("Target Socket Local Pos: %s"), *TargetSocketLocalPos.ToString());
    UE_LOG(CastleLog, Warning, TEXT("Required Level Position: %s"), *RequiredLevelPosition.ToString());
    
    return ResultTransform;
}

ULevelStreaming* UCastleSubsystem::GetLevelByName(const FName& LevelName) const
{
   if (UWorld* World = GetWorld())
   {
       for (ULevelStreaming* Level : World->GetStreamingLevels())
       {
           if (Level && Level->GetWorldAssetPackageName().Contains(LevelName.ToString()))
           {
               return Level;
           }
       }
   }
   return nullptr;
}

FName UCastleSubsystem::GetLevelNameForRoom(const FString& RoomID)
{
   if (!CastleWorldSettings || !CastleWorldSettings->RoomData) return NAME_None;

   return CastleWorldSettings->RoomData->GetRandomLevelVariantForRoom(RoomID);
}

void UCastleSubsystem::OnRoomLoaded()
{
    ProcessingQueue.RemoveAt(0);
    
    if (ProcessingQueue.Num() > 0)
    {
        FString NextRoomID = ProcessingQueue[0];
        FString ConnectToRoomID;
        FName SourceSocket = NAME_None;
        FName TargetSocket = NAME_None;

        // Find a loaded room that this connects to
        for (const auto& LoadedRoom : LoadedRooms)
        {
            TArray<FString> ConnectedRooms = CastleWorldSettings->RoomData->GetConnectedRooms(LoadedRoom.Key);
            if (ConnectedRooms.Contains(NextRoomID))
            {
                ConnectToRoomID = LoadedRoom.Key;
                
                // Get all connection points for both rooms
                TArray<FName> SourcePoints = CastleWorldSettings->RoomData->GetRoomConnectionPoints(ConnectToRoomID);
                TArray<FName> TargetPoints = CastleWorldSettings->RoomData->GetRoomConnectionPoints(NextRoomID);
                
                if (SourcePoints.Num() > 0 && TargetPoints.Num() > 0)
                {
                    // For rooms with multiple connection points, use the last one as source
                    // and first one as target to establish direction
                    SourceSocket = SourcePoints.Num() > 1 ? SourcePoints.Last() : SourcePoints[0];
                    TargetSocket = TargetPoints[0];
                }
                
                break;
            }
        }

        LoadRoom(NextRoomID, ConnectToRoomID, SourceSocket, TargetSocket);
    }
}

void UCastleSubsystem::OnRoomUnloaded()
{
    if (UWorld* World = GetWorld())
    {
        // Find the room that was just unloaded by checking which level is no longer valid
        FString UnloadedRoomID;
        for (auto It = LoadedRooms.CreateIterator(); It; ++It)
        {
            if (ULevelStreaming* Level = GetLevelByName(It->Value.LevelName))
            {
                if (!Level->IsLevelLoaded())
                {
                    UnloadedRoomID = It->Key;
                    LoadedRooms.Remove(It->Key);
                    UE_LOG(CastleLog, Log, TEXT("Room %s successfully unloaded"), *UnloadedRoomID);
                    break;
                }
            }
        }
    }
}

void UCastleSubsystem::DebugDrawSockets()
{
   if (UWorld* World = GetWorld())
   {
       for (const auto& RoomPair : LoadedRooms)
       {
           ULevelStreaming* Level = GetLevelByName(RoomPair.Value.LevelName);
           if (!Level || !Level->GetLoadedLevel()) continue;

           for (AActor* Actor : Level->GetLoadedLevel()->Actors)
           {
               if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
               {
                   DrawDebugSphere(
                       World,
                       SocketActor->GetActorLocation(),
                       50.0f,
                       16,
                       FColor::Yellow,
                       false,
                       5.0f
                   );

                   DrawDebugDirectionalArrow(
                       World,
                       SocketActor->GetActorLocation(),
                       SocketActor->GetActorLocation() + 
                           SocketActor->GetActorForwardVector() * 100.0f,
                       50.0f,
                       FColor::Blue,
                       false,
                       5.0f
                   );
               }
           }
       }
   }
}

void UCastleSubsystem::LogSocketInfo(const FString& RoomID)
{
    if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
    {
        ULevelStreaming* Level = GetLevelByName(RoomInfo->LevelName);
        if (Level && Level->GetLoadedLevel())
        {
            for (AActor* Actor : Level->GetLoadedLevel()->Actors)
            {
                if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
                {
                    UE_LOG(CastleLog, Log, TEXT("Room %s has socket %s at location %s"), 
                        *RoomID,
                        *SocketActor->SocketName.ToString(),
                        *SocketActor->GetActorLocation().ToString());
                }
            }
        }
    }
}

void UCastleSubsystem::ProcessPendingConnections()
{
    // Process any pending connections
    for (int32 i = PendingConnections.Num() - 1; i >= 0; --i)
    {
        const FPendingRoomConnection& Connection = PendingConnections[i];
        
        // Only process if both rooms are loaded
        if (IsRoomLoaded(Connection.RoomID) && IsRoomLoaded(Connection.ConnectedToRoomID))
        {
            FTransform NewTransform = CalculateRoomTransform(
                Connection.ConnectedToRoomID, 
                Connection.RoomID,
                Connection.ConnectFromSocket, 
                Connection.ConnectToSocket);

            // Update room position
            if (ULevelStreaming* Level = GetLevelByName(
                LoadedRooms.Find(Connection.RoomID)->LevelName))
            {
                Level->LevelTransform = NewTransform;
                PendingConnections.RemoveAt(i);
                
                UE_LOG(CastleLog, Log, TEXT("Updated transform for room %s connecting to %s"), 
                    *Connection.RoomID,
                    *Connection.ConnectedToRoomID);

                // Optional: Refresh the level's position immediately
                if (UWorld* World = GetWorld())
                {
                    World->UpdateLevelStreaming();
                }
            }
        }
    }
}

void UCastleSubsystem::DebugLogAllSocketActors(const FString& RoomID)
{
    if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
    {
        UE_LOG(CastleLog, Warning, TEXT("=== Scanning for socket actors in room %s (Level: %s) ==="), 
            *RoomID, *RoomInfo->LevelName.ToString());

        ULevelStreaming* Level = GetLevelByName(RoomInfo->LevelName);
        if (Level && Level->GetLoadedLevel())
        {
            int32 SocketCount = 0;
            for (AActor* Actor : Level->GetLoadedLevel()->Actors)
            {
                if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
                {
                    SocketCount++;
                    UE_LOG(CastleLog, Warning, TEXT("Found Socket Actor %d:"), SocketCount);
                    UE_LOG(CastleLog, Warning, TEXT("  - Name: %s"), *SocketActor->GetName());
                    UE_LOG(CastleLog, Warning, TEXT("  - Socket Name: %s"), *SocketActor->SocketName.ToString());
                    UE_LOG(CastleLog, Warning, TEXT("  - Location: %s"), *SocketActor->GetActorLocation().ToString());
                    UE_LOG(CastleLog, Warning, TEXT("  - Rotation: %s"), *SocketActor->GetActorRotation().ToString());
                }
            }
            
            if (SocketCount == 0)
            {
                UE_LOG(CastleLog, Warning, TEXT("No socket actors found in this level"));
            }
            else
            {
                UE_LOG(CastleLog, Warning, TEXT("Total socket actors found: %d"), SocketCount);
            }
        }
    }
}

void UCastleSubsystem::ClearCompleteCastle()
{
    UE_LOG(CastleLog, Log, TEXT("Starting complete castle cleanup"));
    
    if (UWorld* World = GetWorld())
    {
        // First unload all rooms
        TArray<FString> RoomIDsToUnload = GetLoadedRoomIDs();
        
        if (RoomIDsToUnload.Num() > 0)
        {
            UE_LOG(CastleLog, Log, TEXT("Unloading %d rooms"), RoomIDsToUnload.Num());
            
            // Unload each room
            for (const FString& RoomID : RoomIDsToUnload)
            {
                UnloadRoom(RoomID);
            }
            
            // Set up a timer to check when all rooms are unloaded
            FTimerHandle TimerHandle;
            World->GetTimerManager().SetTimer(TimerHandle, 
                FTimerDelegate::CreateLambda([this]()
                {
                    if (!AreAnyRoomsUnloading())
                    {
                        UE_LOG(CastleLog, Log, TEXT("All rooms unloaded, clearing internal state"));
                        
                        // Clear all internal state
                        LoadedRooms.Empty();
                        ProcessingQueue.Empty();
                        PendingConnections.Empty();
                        PendingRoomLoads.Empty();
                        
                        UE_LOG(CastleLog, Log, TEXT("Castle cleanup complete"));
                    }
                }), 0.1f, true); // Check every 0.1 seconds
        }
        else
        {
            // If no rooms to unload, just clear the state
            LoadedRooms.Empty();
            ProcessingQueue.Empty();
            PendingConnections.Empty();
            PendingRoomLoads.Empty();
            
            UE_LOG(CastleLog, Log, TEXT("Castle cleanup complete"));
        }
    }
}

bool UCastleSubsystem::AreAnyRoomsUnloading() const
{
    if (UWorld* World = GetWorld())
    {
        // Check all streaming levels
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

void UCastleSubsystem::OnGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
    // We're specifically interested in the Resetting state
    if (NewState == EDkGameState::Resetting)
    {
        UE_LOG(CastleLog, Log, TEXT("Castle System detected reset state - Beginning castle reset sequence"));
        
        // Start the reset sequence
        ClearCompleteCastle();
    }
}

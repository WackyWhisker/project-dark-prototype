// Copyright @ Christian Reichel

#include "Subsystem/CastleSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Data/CastleRoomData.h"
#include "Engine/LevelStreamingDynamic.h"
#include "WorldSetting/CastleWorldSettings.h"
#include "DrawDebugHelpers.h"
#include "Helper/GameLogging.h"
#include "Subsystem/CastleSocketActor.h"
#include "Subsystem/DkGameStateSubsystem.h"

void UCastleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    // Initialize dependency first
    Collection.InitializeDependency<UDkGameStateSubsystem>();
    
    Super::Initialize(Collection);

    CASTLE_LOG(Warning, TEXT("Castle Subsystem initialization starting"));

    if (UWorld* World = GetWorld())
    {
        GameStateSubsystem = World->GetSubsystem<UDkGameStateSubsystem>();
        if (GameStateSubsystem)
        {
            //CASTLE_LOG(Warning, TEXT("Found GameStateSubsystem, subscribing to events"));
            GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UCastleSubsystem::OnGameStateChanged);
        }
        else
        {
            //CASTLE_LOG(Error, TEXT("GameStateSubsystem not found during Castle initialization - This should never happen due to dependency resolution"));
        }
    }

    CastleWorldSettings = Cast<ACastleWorldSettings>(GetWorld()->GetWorldSettings());
    if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
    {
        //CASTLE_LOG(Warning, TEXT("Failed to initialize Castle Subsystem - Missing World Settings or Room Data"));
        return;
    }

    bool bSuccess = CastleWorldSettings->RoomData->LoadFromContentJSON("Data/castle_data.json");
    if (!bSuccess)
    {
        //CASTLE_LOG(Error, TEXT("Failed to load castle data from JSON"));
        return;
    }

    LoadedRooms.Empty();
    ProcessingQueue.Empty();
}

void UCastleSubsystem::TestSubsystemAccess()
{
   CASTLE_LOG(Warning, TEXT("Castle Subsystem accessed successfully"));
}

void UCastleSubsystem::SpawnCompleteCastle()
{
    if (bIsCastleSpawning) return;
    
    if (!CastleWorldSettings || !CastleWorldSettings->RoomData)
    {
        CASTLE_LOG(Error, TEXT("Cannot spawn castle - Missing Room Data"));
        return;
    }

    bIsCastleSpawning = true;

    // Clear any existing state
    LoadedRooms.Empty();
    ProcessingQueue.Empty();

    // Find entry room and start the sequence
    FString EntryRoomID = FindEntryRoomID();
    if (EntryRoomID.IsEmpty())
    {
        CASTLE_LOG(Error, TEXT("No entry room found in castle data"));
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

                CASTLE_LOG(Warning, TEXT("=== Loading Room: %s ==="), *LocalRoomID);
                CASTLE_LOG(Warning, TEXT("Connected To: %s"), LocalConnectedToRoomID.IsEmpty() ? TEXT("None") : *LocalConnectedToRoomID);
                CASTLE_LOG(Warning, TEXT("Connect From Socket: %s"), *LocalConnectFromSocket.ToString());
                CASTLE_LOG(Warning, TEXT("Connect To Socket: %s"), *LocalConnectToSocket.ToString());

                // If this isn't the entry room, get the world position from the connected room's socket
                if (!LocalConnectedToRoomID.IsEmpty())
                {
                    FLoadedRoomInfo* ConnectedRoom = LoadedRooms.Find(LocalConnectedToRoomID);
                    if (ConnectedRoom)
                    {
                        CASTLE_LOG(Warning, TEXT("Found Connected Room: %s (Level: %s)"), 
                            *ConnectedRoom->RoomID, *ConnectedRoom->LevelName.ToString());

                        ULevelStreaming* ConnectedLevel = GetLevelByName(ConnectedRoom->LevelName);
                        if (ConnectedLevel && ConnectedLevel->GetLoadedLevel())
                        {
                            CASTLE_LOG(Warning, TEXT("Connected Level Transform: %s"), 
                                *ConnectedLevel->LevelTransform.ToString());

                            // Find the socket in the connected room
                            bool bFoundSocket = false;
                            for (AActor* Actor : ConnectedLevel->GetLoadedLevel()->Actors)
                            {
                                if (ACastleSocketActor* SocketActor = Cast<ACastleSocketActor>(Actor))
                                {
                                    FVector ActorLocation = SocketActor->GetActorLocation();
                                    FTransform ActorTransform = SocketActor->GetActorTransform();
        
                                    CASTLE_LOG(Warning, TEXT("Found Socket: %s"), *SocketActor->SocketName.ToString());
                                    CASTLE_LOG(Warning, TEXT("  Actor Location: %s"), *ActorLocation.ToString());
                                    CASTLE_LOG(Warning, TEXT("  Actor Transform: %s"), *ActorTransform.ToString());
                                    CASTLE_LOG(Warning, TEXT("  Level Transform: %s"), 
                                        *ConnectedLevel->LevelTransform.ToString());

                                    if (SocketActor->SocketName == LocalConnectFromSocket)
                                    {
                                        SpawnPosition = ActorLocation;
                                        SpawnRotation = SocketActor->GetActorRotation() + FRotator(0, 0.0f, 0);
            
                                        CASTLE_LOG(Warning, TEXT("Using Socket for Spawn - Position: %s, Rotation: %s"), 
                                            *SpawnPosition.ToString(), *SpawnRotation.ToString());
            
                                        bFoundSocket = true;
                                        break;
                                    }
                                }
                            }
                            
                            if (!bFoundSocket)
                            {
                                CASTLE_LOG(Error, TEXT("Failed to find socket %s in room %s"), 
                                    *LocalConnectFromSocket.ToString(), *LocalConnectedToRoomID);
                            }
                        }
                        else
                        {
                            CASTLE_LOG(Error, TEXT("Connected level not found or not loaded"));
                        }
                    }
                    else
                    {
                        CASTLE_LOG(Error, TEXT("Connected room info not found: %s"), *LocalConnectedToRoomID);
                    }
                }
                else
                {
                    CASTLE_LOG(Warning, TEXT("Entry room - spawning at origin"));
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
                    
                    CASTLE_LOG(Warning, TEXT("=== Room %s spawn complete ==="), *LocalRoomID);
                    CASTLE_LOG(Warning, TEXT("Final Position: %s"), *SpawnPosition.ToString());
                    CASTLE_LOG(Warning, TEXT("Final Rotation: %s"), *SpawnRotation.ToString());
                }
                else
                {
                    CASTLE_LOG(Error, TEXT("Failed to load level instance for room: %s"), *LocalRoomID);
                }
            }
        }, 0.5f, false);
    }
}

void UCastleSubsystem::UnloadRoom(const FString& RoomID)
{
    CASTLE_LOG(Warning, TEXT("UnloadRoom - Attempting to unload room: %s"), *RoomID);
    
    if (UWorld* World = GetWorld())
    {
        if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
        {
            if (ULevelStreaming* Level = GetLevelByName(RoomInfo->LevelName))
            {
                // Get the full package name from the streaming level
                FString PackageName = Level->GetWorldAssetPackageName();
                
                CASTLE_LOG(Warning, TEXT("  Found streaming level, unloading package: %s"), *PackageName);
                
                FLatentActionInfo LatentInfo;
                LatentInfo.CallbackTarget = this;
                LatentInfo.ExecutionFunction = "OnRoomUnloaded";
                LatentInfo.Linkage = 0;
                LatentInfo.UUID = FMath::Rand();

                Level->OnLevelUnloaded.AddDynamic(this, &UCastleSubsystem::OnRoomUnloaded);
                UGameplayStatics::UnloadStreamLevel(World, FName(*PackageName), LatentInfo, false);
            }
            else
            {
                CASTLE_LOG(Error, TEXT("  Failed to find streaming level for room: %s"), *RoomID);
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

    CASTLE_LOG(Warning, TEXT("=== Transform Calculation ==="));
    CASTLE_LOG(Warning, TEXT("Source Socket World Pos: %s"), *SourceSocketWorldPos.ToString());
    CASTLE_LOG(Warning, TEXT("Target Socket Local Pos: %s"), *TargetSocketLocalPos.ToString());
    CASTLE_LOG(Warning, TEXT("Required Level Position: %s"), *RequiredLevelPosition.ToString());
    
    return ResultTransform;
}

ULevelStreaming* UCastleSubsystem::GetLevelByName(const FName& LevelName) const
{
    if (UWorld* World = GetWorld())
    {
        CASTLE_LOG(Warning, TEXT("=== GetLevelByName Detailed Analysis ==="));
        CASTLE_LOG(Warning, TEXT("Searching for level: %s"), *LevelName.ToString());
        
        TArray<ULevelStreaming*> StreamingLevels = World->GetStreamingLevels();
        CASTLE_LOG(Warning, TEXT("Found %d streaming levels in world"), StreamingLevels.Num());
        
        for (ULevelStreaming* Level : StreamingLevels)
        {
            if (Level)
            {
                FString PackageName = Level->GetWorldAssetPackageName();
                CASTLE_LOG(Warning, TEXT("\nAnalyzing Level:"));
                CASTLE_LOG(Warning, TEXT("- Package Name: %s"), *PackageName);
                CASTLE_LOG(Warning, TEXT("- Is Loaded: %s"), Level->IsLevelLoaded() ? TEXT("Yes") : TEXT("No"));
                CASTLE_LOG(Warning, TEXT("- Is Visible: %s"), Level->IsLevelVisible() ? TEXT("Yes") : TEXT("No"));
                CASTLE_LOG(Warning, TEXT("- Streaming State: %s"), 
                    Level->IsStreamingStatePending() ? TEXT("Pending") : TEXT("Stable"));
                CASTLE_LOG(Warning, TEXT("- Current State: %d"), static_cast<int>(Level->GetLevelStreamingState()));

                
                if (Level->GetLoadedLevel())
                {
                    CASTLE_LOG(Warning, TEXT("- Loaded Level Info:"));
                    CASTLE_LOG(Warning, TEXT("  - Level Name: %s"), *Level->GetLoadedLevel()->GetName());
                    CASTLE_LOG(Warning, TEXT("  - Outer Name: %s"), *Level->GetLoadedLevel()->GetOuter()->GetName());
                    CASTLE_LOG(Warning, TEXT("  - Transform: %s"), *Level->LevelTransform.ToString());
                }
                else
                {
                    CASTLE_LOG(Warning, TEXT("- Loaded Level: None"));
                }
                
                if (PackageName.Contains(LevelName.ToString()))
                {
                    CASTLE_LOG(Warning, TEXT("Found matching level!"));
                    CASTLE_LOG(Warning, TEXT("=== End Analysis ===\n"));
                    return Level;
                }
            }
            else
            {
                CASTLE_LOG(Warning, TEXT("\nFound null streaming level entry"));
            }
        }
        CASTLE_LOG(Warning, TEXT("No matching level found for: %s"), *LevelName.ToString());
        CASTLE_LOG(Warning, TEXT("=== End Analysis ===\n"));
    }
    else
    {
        CASTLE_LOG(Error, TEXT("GetLevelByName - No valid World"));
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
                    CASTLE_LOG(Log, TEXT("Room %s successfully unloaded"), *UnloadedRoomID);
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
                    CASTLE_LOG(Log, TEXT("Room %s has socket %s at location %s"), 
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
                
                CASTLE_LOG(Log, TEXT("Updated transform for room %s connecting to %s"), 
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
        CASTLE_LOG(Warning, TEXT("=== Scanning for socket actors in room %s (Level: %s) ==="), 
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
                    CASTLE_LOG(Warning, TEXT("Found Socket Actor %d:"), SocketCount);
                    CASTLE_LOG(Warning, TEXT("  - Name: %s"), *SocketActor->GetName());
                    CASTLE_LOG(Warning, TEXT("  - Socket Name: %s"), *SocketActor->SocketName.ToString());
                    CASTLE_LOG(Warning, TEXT("  - Location: %s"), *SocketActor->GetActorLocation().ToString());
                    CASTLE_LOG(Warning, TEXT("  - Rotation: %s"), *SocketActor->GetActorRotation().ToString());
                }
            }
            
            if (SocketCount == 0)
            {
                CASTLE_LOG(Warning, TEXT("No socket actors found in this level"));
            }
            else
            {
                CASTLE_LOG(Warning, TEXT("Total socket actors found: %d"), SocketCount);
            }
        }
    }
}

void UCastleSubsystem::ClearCompleteCastle()
{
    CASTLE_LOG(Log, TEXT("Starting complete castle cleanup"));
    
    if (UWorld* World = GetWorld())
    {
        // Get all streaming levels before we start removing them
        TArray<ULevelStreaming*> LevelsToRemove = World->GetStreamingLevels();
        CASTLE_LOG(Warning, TEXT("Found %d streaming levels to clean up"), LevelsToRemove.Num());
        
        // First unload all rooms and mark them for removal
        TArray<FString> RoomIDsToUnload = GetLoadedRoomIDs();
        
        if (RoomIDsToUnload.Num() > 0)
        {
            CASTLE_LOG(Log, TEXT("Unloading %d rooms"), RoomIDsToUnload.Num());
            
            // Unload each room
            for (const FString& RoomID : RoomIDsToUnload)
            {
                UnloadRoom(RoomID);
            }
            
            // Clear any existing timer first
            World->GetTimerManager().ClearTimer(CleanupTimerHandle);
            
            // Set up new timer that will also handle level removal
            World->GetTimerManager().SetTimer(CleanupTimerHandle, 
                FTimerDelegate::CreateLambda([this, World, LevelsToRemove]()
                {
                    if (!AreAnyRoomsUnloading())
                    {
                        CASTLE_LOG(Warning, TEXT("All rooms unloaded, removing %d streaming level instances"), LevelsToRemove.Num());
                        
                        // Now explicitly remove each streaming level
                        for (ULevelStreaming* Level : LevelsToRemove)
                        {
                            if (Level)
                            {
                                CASTLE_LOG(Warning, TEXT("Removing streaming level: %s"), *Level->GetWorldAssetPackageName());
                                World->RemoveStreamingLevel(Level);
                            }
                        }
                        
                        // Force a GC pass to clean up any remaining references
                        GEngine->ForceGarbageCollection(true);
                        
                        ClearInternalState();
                        
                        // Clear our own timer
                        World->GetTimerManager().ClearTimer(CleanupTimerHandle);
                    }
                }), 0.1f, true);
        }
        else
        {
            // If no rooms to unload, just clear the state
            ClearInternalState();
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
        CASTLE_LOG(Log, TEXT("Castle System detected reset state - Beginning castle reset sequence"));
        // Register ourselves for reset tracking
        if (GameStateSubsystem)
        {
            GameStateSubsystem->RegisterForReset(this);
        }
        
        // Start the reset sequence
        ClearCompleteCastle();
    }
}

void UCastleSubsystem::CheckUnloadProgress()
{
    if (!AreAnyRoomsUnloading())
    {
        CASTLE_LOG(Log, TEXT("All rooms unloaded, clearing internal state"));
        ClearInternalState();
        
        // Clear the timer
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(CleanupTimerHandle);
        }
    }
}

void UCastleSubsystem::ClearInternalState()
{
    LoadedRooms.Empty();
    ProcessingQueue.Empty();
    PendingConnections.Empty();
    PendingRoomLoads.Empty();
    bIsCastleSpawning = false;
    
    // Notify game state that we're done with reset
    if (GameStateSubsystem)
    {
        GameStateSubsystem->NotifyResetComplete(this);
    }
    CASTLE_LOG(Log, TEXT("Castle cleanup complete"));
}

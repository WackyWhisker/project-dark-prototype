#include "LevelObject/MegaComplexSpawner.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LevelObject/RoomSocketActor.h"

AMegaComplexSpawner::AMegaComplexSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsSpawning = false;
}

void AMegaComplexSpawner::BeginPlay()
{
    Super::BeginPlay();
}

void AMegaComplexSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ClearCompleteMegaComplex();
}

void AMegaComplexSpawner::SpawnCompleteMegaComplex()
{
    if (bIsSpawning || !RoomData) 
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnCompleteMegaComplex: Already spawning or no RoomData"));
        return;
    }

    bIsSpawning = true;
    UE_LOG(LogTemp, Log, TEXT("Starting MegaComplex spawn"));

    LoadedRooms.Empty();
    ProcessingQueue.Empty();
    PendingConnections.Empty();

    FString EntryRoomID = FindEntryRoomID();
    if (EntryRoomID.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No entry room found!"));
        bIsSpawning = false;
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Found entry room: %s"), *EntryRoomID);
    
    // Build the queue in the correct order
    ProcessingQueue.Add(EntryRoomID);
    int32 CurrentIndex = 0;

    while (CurrentIndex < ProcessingQueue.Num())
    {
        FString CurrentRoomID = ProcessingQueue[CurrentIndex];
        TArray<FString> NextRooms = GetNextRooms(CurrentRoomID);
        
        for (const FString& NextRoomID : NextRooms)
        {
            if (!ProcessingQueue.Contains(NextRoomID))
            {
                ProcessingQueue.Add(NextRoomID);
                UE_LOG(LogTemp, Log, TEXT("Added room to queue: %s"), *NextRoomID);
            }
        }
        
        CurrentIndex++;
    }

    UE_LOG(LogTemp, Log, TEXT("Total rooms in queue: %d"), ProcessingQueue.Num());

    // Start with the first room
    if (ProcessingQueue.Num() > 0)
    {
        LoadRoom(ProcessingQueue[0]);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No rooms in processing queue after setup"));
        bIsSpawning = false;
    }
}

void AMegaComplexSpawner::LoadEntryRoom()
{
    if (!RoomData) return;

    TArray<FString> EntryRooms = RoomData->GetRoomIDsOfType(ERoomType::Entry);
    if (EntryRooms.Num() > 0)
    {
        LoadRoom(EntryRooms[0]);
    }
}

void AMegaComplexSpawner::LoadRandomConnectionRoom(const FString& ConnectedToRoomID)
{
    if (!RoomData) return;

    TArray<FString> ConnectionRooms = RoomData->GetRoomIDsOfType(ERoomType::Connection);
    if (ConnectionRooms.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, ConnectionRooms.Num() - 1);
        LoadRoom(ConnectionRooms[RandomIndex], ConnectedToRoomID);
    }
}

void AMegaComplexSpawner::LoadRoom(const FString& RoomID, const FString& ConnectedToRoomID, 
    ERoomSocketType ConnectFromSocket, ERoomSocketType ConnectToSocket)
{
    if (!RoomData) return;
    
    UE_LOG(LogTemp, Log, TEXT("Attempting to load room: %s"), *RoomID);
    
    if (UWorld* World = GetWorld())
    {
        if (!LoadedRooms.Contains(RoomID))
        {
            TSoftObjectPtr<UWorld> LevelReference = RoomData->GetRandomLevelVariantForRoom(RoomID);
            FString LevelPath = LevelReference.ToString();
            
            if (LevelPath.IsEmpty()) return;

            UE_LOG(LogTemp, Log, TEXT("Creating level instance for path: %s"), *LevelPath);

            bool bSucceeded = false;
            ULevelStreamingDynamic* NewLevel = ULevelStreamingDynamic::LoadLevelInstance(
                World,
                LevelPath,
                GetActorLocation(),
                GetActorRotation(),
                bSucceeded
            );

            if (bSucceeded && NewLevel)
            {
                // Track the loading info
                FRoomLoadingInfo LoadingInfo;
                LoadingInfo.RoomID = RoomID;
                LoadingInfo.LevelReference = LevelReference;
                LoadingInfo.ConnectedToRoomID = ConnectedToRoomID;
                LoadingInfo.ConnectFromSocket = ConnectFromSocket;
                LoadingInfo.ConnectToSocket = ConnectToSocket;
                PendingRoomLoads.Add(NewLevel, LoadingInfo);

                // Bind both delegates 
                FScriptDelegate LoadedDelegate;
                LoadedDelegate.BindUFunction(this, "OnRoomLoaded");
                NewLevel->OnLevelLoaded.Add(LoadedDelegate);

                UE_LOG(LogTemp, Log, TEXT("Level instance created and callback bound for room: %s"), *RoomID);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create level instance for room: %s"), *RoomID);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Room %s is already loaded"), *RoomID);
        }
    }
}

void AMegaComplexSpawner::UnloadRoom(const FString& RoomID)
{
    if (UWorld* World = GetWorld())
    {
        if (FLoadedRoomInfo* RoomInfo = LoadedRooms.Find(RoomID))
        {
            if (ULevelStreaming* Level = GetLevelByReference(RoomInfo->LevelReference))
            {
                FLatentActionInfo LatentInfo;
                LatentInfo.CallbackTarget = this;
                LatentInfo.ExecutionFunction = "OnRoomUnloaded";
                LatentInfo.Linkage = 0;
                LatentInfo.UUID = FMath::Rand();

                Level->OnLevelUnloaded.AddDynamic(this, &AMegaComplexSpawner::OnRoomUnloaded);
                UGameplayStatics::UnloadStreamLevel(World, FName(*RoomInfo->LevelReference.ToString()), 
                    LatentInfo, false);
            }
        }
    }
}

bool AMegaComplexSpawner::IsRoomLoaded(const FString& RoomID) const
{
    return LoadedRooms.Contains(RoomID);
}

TArray<FString> AMegaComplexSpawner::GetLoadedRoomIDs() const
{
    TArray<FString> RoomIDs;
    LoadedRooms.GetKeys(RoomIDs);
    return RoomIDs;
}

void AMegaComplexSpawner::DebugDrawSockets()
{
    // Empty debug visualization
}

void AMegaComplexSpawner::LogRoomConnections()
{
    // Empty logging functionality
}

FString AMegaComplexSpawner::FindEntryRoomID() const
{
    if (!RoomData) return FString();

    TArray<FString> EntryRooms = RoomData->GetRoomIDsOfType(ERoomType::Entry);
    return EntryRooms.Num() > 0 ? EntryRooms[0] : FString();
}

TArray<FString> AMegaComplexSpawner::GetNextRooms(const FString& CurrentRoomID) const
{
    TArray<FString> NextRooms;
    if (!RoomData) return NextRooms;

    TArray<FString> ConnectedRooms = RoomData->GetConnectedRooms(CurrentRoomID);
    for (const FString& ConnectedRoom : ConnectedRooms)
    {
        if (!LoadedRooms.Contains(ConnectedRoom))
        {
            NextRooms.Add(ConnectedRoom);
        }
    }

    return NextRooms;
}

FTransform AMegaComplexSpawner::CalculateRoomTransform(const FString& SourceRoomID, const FString& TargetRoomID, 
    ERoomSocketType SourceSocket, ERoomSocketType TargetSocket)
{
    FLoadedRoomInfo* SourceInfo = LoadedRooms.Find(SourceRoomID);
    if (!SourceInfo) return FTransform::Identity;

    ULevelStreaming* SourceLevel = GetLevelByReference(SourceInfo->LevelReference);
    if (!SourceLevel || !SourceLevel->GetLoadedLevel()) return FTransform::Identity;

    ARoomSocketActor* SourceSocketActor = nullptr;
    for (AActor* Actor : SourceLevel->GetLoadedLevel()->Actors)
    {
        if (ARoomSocketActor* SocketActor = Cast<ARoomSocketActor>(Actor))
        {
            if (SocketActor->SocketType == SourceSocket)
            {
                SourceSocketActor = SocketActor;
                break;
            }
        }
    }

    if (!SourceSocketActor) return FTransform::Identity;

    FLoadedRoomInfo* TargetInfo = LoadedRooms.Find(TargetRoomID);
    if (!TargetInfo) return FTransform::Identity;

    ULevelStreaming* TargetLevel = GetLevelByReference(TargetInfo->LevelReference);
    if (!TargetLevel || !TargetLevel->GetLoadedLevel()) return FTransform::Identity;

    ARoomSocketActor* TargetSocketActor = nullptr;
    for (AActor* Actor : TargetLevel->GetLoadedLevel()->Actors)
    {
        if (ARoomSocketActor* SocketActor = Cast<ARoomSocketActor>(Actor))
        {
            if (SocketActor->SocketType == TargetSocket)
            {
                TargetSocketActor = SocketActor;
                break;
            }
        }
    }

    if (!TargetSocketActor) return FTransform::Identity;

    FVector SourceSocketWorldPos = SourceSocketActor->GetActorLocation();
    FVector TargetSocketLocalPos = TargetSocketActor->GetActorLocation() - TargetLevel->LevelTransform.GetLocation();
    FVector RequiredLevelPosition = SourceSocketWorldPos - TargetSocketLocalPos;

    FTransform ResultTransform;
    ResultTransform.SetLocation(RequiredLevelPosition);
    ResultTransform.SetRotation(FRotator(0, 180.0f, 0).Quaternion());

    return ResultTransform;
}

ULevelStreaming* AMegaComplexSpawner::GetLevelByReference(const TSoftObjectPtr<UWorld>& LevelReference) const
{
    if (UWorld* World = GetWorld())
    {
        for (ULevelStreaming* Level : World->GetStreamingLevels())
        {
            if (Level && Level->GetWorldAsset() == LevelReference)
            {
                return Level;
            }
        }
    }
    return nullptr;
}

void AMegaComplexSpawner::OnRoomLoaded()
{
    UE_LOG(LogTemp, Log, TEXT("OnRoomLoaded called"));

    // Find which level just finished loading
    ULevelStreaming* LoadedLevel = nullptr;
    FRoomLoadingInfo LoadingInfo;
    
    for (auto& Pair : PendingRoomLoads)
    {
        if (Pair.Key && Pair.Key->IsLevelLoaded())
        {
            LoadedLevel = Pair.Key;
            LoadingInfo = Pair.Value;
            UE_LOG(LogTemp, Log, TEXT("Found loaded level for room: %s"), *LoadingInfo.RoomID);
            break;
        }
    }

    if (!LoadedLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnRoomLoaded: No loaded level found"));
        return;
    }

    // Add to loaded rooms
    FLoadedRoomInfo RoomInfo;
    RoomInfo.RoomID = LoadingInfo.RoomID;
    RoomInfo.LevelReference = LoadingInfo.LevelReference;
    RoomInfo.RoomType = RoomData->GetRoomType(LoadingInfo.RoomID);
    LoadedRooms.Add(LoadingInfo.RoomID, RoomInfo);

    // Remove from pending loads
    PendingRoomLoads.Remove(LoadedLevel);

    UE_LOG(LogTemp, Log, TEXT("Room %s fully loaded and added to LoadedRooms"), *LoadingInfo.RoomID);

    // Find the current room in the queue and remove it
    if (ProcessingQueue.Num() > 0 && ProcessingQueue[0] == LoadingInfo.RoomID)
    {
        ProcessingQueue.RemoveAt(0);
    }

    // Process next room in queue
    if (ProcessingQueue.Num() > 0)
    {
        FString NextRoomID = ProcessingQueue[0];
        UE_LOG(LogTemp, Log, TEXT("Loading next room in queue: %s"), *NextRoomID);
        LoadRoom(NextRoomID, LoadingInfo.RoomID,
            ERoomSocketType::Socket_Entry,
            ERoomSocketType::Socket_Exit);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No more rooms in queue. Spawn complete."));
        bIsSpawning = false;
    }
}

void AMegaComplexSpawner::OnRoomUnloaded()
{
    if (UWorld* World = GetWorld())
    {
        for (auto It = LoadedRooms.CreateIterator(); It; ++It)
        {
            if (ULevelStreaming* Level = GetLevelByReference(It->Value.LevelReference))
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

void AMegaComplexSpawner::ClearCompleteMegaComplex()
{
    TArray<FString> RoomIDsToUnload = GetLoadedRoomIDs();
    
    for (const FString& RoomID : RoomIDsToUnload)
    {
        UnloadRoom(RoomID);
    }

    LoadedRooms.Empty();
    ProcessingQueue.Empty();
    PendingConnections.Empty();
    PendingRoomLoads.Empty();
    bIsSpawning = false;
}


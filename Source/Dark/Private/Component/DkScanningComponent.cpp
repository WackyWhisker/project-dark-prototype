#include "Component/DkScanningComponent.h"
#include "Character/DkCharacter.h"
#include "Component/DkScannableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerControllerInterface.h"

UDkScanningComponent::UDkScanningComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDkScanningComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!PlayerControllerInterface)
    {
        PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
    }

    if (!PlayerRef)
    {
        PlayerRef = Cast<ADkCharacter>(GetOwner());
    }

    if (PlayerControllerInterface)
    {
        PlayerControllerInterface->GetScanModeStartDelegate()->AddUObject(this, &UDkScanningComponent::OnScanModeStart);
        PlayerControllerInterface->GetScanModeEndDelegate()->AddUObject(this, &UDkScanningComponent::OnScanModeEnd);
        PlayerControllerInterface->GetScanExecuteStartDelegate()->AddUObject(this, &UDkScanningComponent::OnScanExecuteStart);
        PlayerControllerInterface->GetScanExecuteEndDelegate()->AddUObject(this, &UDkScanningComponent::OnScanExecuteEnd);
    }
}

void UDkScanningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
  
    if (PlayerControllerInterface)
    {
        PlayerControllerInterface->GetScanModeStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanModeEndDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanExecuteStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanExecuteEndDelegate()->RemoveAll(this);
    }
}

void UDkScanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsInScanMode)
    {
        return;
    }

    // If we're in scan mode but not actively scanning, keep updating target
    if (!bIsExecutingScanning)
    {
        InitiateSweepForScannables();
        return;
    }

    // If we're actively scanning, handle progress
    if (!CurrentScannableTarget)
    {
        return;
    }

    float CurrentProgress = CurrentScannableTarget->GetCurrentScanProgress();
    float ScanDuration = CurrentScannableTarget->GetScanDuration();
   
    CurrentProgress += DeltaTime / ScanDuration;
    CurrentProgress = FMath::Clamp(CurrentProgress, 0.0f, 1.0f);
   
    CurrentScannableTarget->OnScanProgress(CurrentProgress);

    if (CurrentProgress >= 1.0f)
    {
        CurrentScannableTarget->OnScanComplete();
        CurrentScannableTarget = nullptr;
        bIsExecutingScanning = false;
        PrimaryComponentTick.bCanEverTick = false;
    }
}

void UDkScanningComponent::OnScanModeStart()
{
    if (!bIsInScanMode)
    {
        UE_LOG(LogTemp, Log, TEXT("Scan Mode Started"));
        bIsInScanMode = true;
        InitiateSweepForScannables();
      
        TArray<UDkScannableComponent*> ScannablesInRange;
        if (GetScannableComponentsInRange(ScannablesInRange, ScannableSearchRange))
        {
            for (UDkScannableComponent* Scannable : ScannablesInRange)
            {
                if (Scannable)
                {
                    Scannable->OnScanModeEntered();
                }
            }
        }
    }
}

void UDkScanningComponent::OnScanModeEnd()
{
    if (bIsInScanMode)
    {
        UE_LOG(LogTemp, Log, TEXT("Scan Mode Ended"));
        bIsInScanMode = false;
       
        if (bIsExecutingScanning)
        {
            OnScanExecuteEnd();
        }

        TArray<UDkScannableComponent*> ScannablesInRange;
        if (GetScannableComponentsInRange(ScannablesInRange, ScannableSearchRange))
        {
            for (UDkScannableComponent* Scannable : ScannablesInRange)
            {
                if (Scannable)
                {
                    Scannable->OnScanModeExited();
                }
            }
        }

        CurrentScannableTarget = nullptr;
    }
}

void UDkScanningComponent::OnScanExecuteStart()
{
    if (bIsInScanMode && CurrentScannableTarget && !bIsExecutingScanning)
    {
        UE_LOG(LogTemp, Log, TEXT("Scan Execute Started"));
        bIsExecutingScanning = true;
        CurrentScannableTarget->OnScanStart();
        PrimaryComponentTick.bCanEverTick = true;
    }
}

void UDkScanningComponent::OnScanExecuteEnd()
{
    if (bIsExecutingScanning)
    {
        UE_LOG(LogTemp, Log, TEXT("Scan Execute Ended"));
        bIsExecutingScanning = false;

        if (CurrentScannableTarget)
        {
            CurrentScannableTarget->OnScanAborted();
        }
       
        PrimaryComponentTick.bCanEverTick = false;
    }
}

bool UDkScanningComponent::FindBestScannable(UDkScannableComponent*& OutBestScannable, float SphereRadius, float MaxAngleInDegrees)
{
    TArray<UDkScannableComponent*> ScannablesInRange;
    TArray<UDkScannableComponent*> ScannablesInCone;
  
    if (!GetScannableComponentsInRange(ScannablesInRange, SphereRadius))
    {
        return false;
    }
  
    if (!FilterScannablesByViewCone(ScannablesInRange, ScannablesInCone, MaxAngleInDegrees))
    {
        return false;
    }

    OutBestScannable = ScannablesInCone[0];
    return true;
}

bool UDkScanningComponent::GetScannableComponentsInRange(TArray<UDkScannableComponent*>& OutScannables, float SphereRadius)
{
    if (!PlayerRef) { return false; }

    FVector StartLocation = PlayerRef->GetActorLocation();

    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(PlayerRef);

    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(SphereRadius);

    bool bSuccess = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        StartLocation,
        FQuat::Identity,
        ECollisionChannel::ECC_Visibility,
        CollisionShape,
        QueryParams
    );
  
    if (bSuccess)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AActor* OverlappedActor = Result.GetActor())
            {
                if (UDkScannableComponent* ScannableComp = OverlappedActor->FindComponentByClass<UDkScannableComponent>())
                {
                    OutScannables.Add(ScannableComp);
                }
            }
        }
    }

    return OutScannables.Num() > 0;
}

bool UDkScanningComponent::FilterScannablesByViewCone(const TArray<UDkScannableComponent*>& PotentialScannables,
   TArray<UDkScannableComponent*>& OutValidScannables, float MaxAngleInDegrees)
{
    if (!PlayerRef) { return false; }
  
    FVector PlayerLocation = PlayerRef->GetActorLocation();
    FVector PlayerForward = PlayerRef->GetActorForwardVector();
  
    float CosMaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxAngleInDegrees));

    struct FScanScore 
    {
        UDkScannableComponent* Component;
        float Score;
        float Distance;
  
        FScanScore(UDkScannableComponent* InComponent, float InScore, float InDistance)
            : Component(InComponent), Score(InScore), Distance(InDistance) {}
    };

    TArray<FScanScore> ScoredScannables;
  
    for (UDkScannableComponent* Scannable : PotentialScannables)
    {
        if (!Scannable) continue;
        
        AActor* ScannableOwner = Scannable->GetOwner();
        if (!ScannableOwner) continue;

        FVector ScannableLocation = ScannableOwner->GetActorLocation();
        FVector DirectionToScannable = (ScannableLocation - PlayerLocation);
        float Distance = DirectionToScannable.Size();
        DirectionToScannable.Normalize();

        float DotProduct = FVector::DotProduct(PlayerForward, DirectionToScannable);
      
        if (DotProduct >= CosMaxAngle)
        {
            // Score based on both angle and distance
            float AngleScore = (DotProduct - CosMaxAngle) / (1.0f - CosMaxAngle); // 0 to 1
            float DistanceScore = 1.0f - FMath::Min(Distance / ScannableSearchRange, 1.0f); // 1 when close, 0 when far
            float FinalScore = (AngleScore * 0.5f) + (DistanceScore * 0.5f); // Weighted combination

            ScoredScannables.Add(FScanScore(Scannable, FinalScore, Distance));

            // Debug draw
            DrawDebugLine(GetWorld(), PlayerLocation, ScannableLocation, 
                FColor::Yellow, false, 0.1f, 0, 2.0f);
            DrawDebugString(GetWorld(), ScannableLocation, 
                FString::Printf(TEXT("Score: %.2f"), FinalScore),
                nullptr, FColor::White, 0.1f);
        }
    }
  
    // Sort by score
    ScoredScannables.Sort([](const FScanScore& A, const FScanScore& B) {
        return A.Score > B.Score;
    });
  
    OutValidScannables.Empty();
    for (const FScanScore& ScoredScannable : ScoredScannables)
    {
        OutValidScannables.Add(ScoredScannable.Component);
    }
  
    return OutValidScannables.Num() > 0;
}

void UDkScanningComponent::InitiateSweepForScannables()
{
    UDkScannableComponent* NewScannable = nullptr;
    if (FindBestScannable(NewScannable, ScannableSearchRange, MaxViewConeAngle))
    {
        CurrentScannableTarget = NewScannable;
        UE_LOG(LogTemp, Log, TEXT("Found scannable target: %s"), *NewScannable->GetOwner()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No scannable found in range"));
    }
}
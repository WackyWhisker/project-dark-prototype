#include "Component/DkScanningComponent.h"
#include "Character/DkCharacter.h"
#include "Component/DkScannableComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerController.h"
#include "Player/DkPlayerControllerInterface.h"

UDkScanningComponent::UDkScanningComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDkScanningComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!PlayerRef)
    {
        PlayerRef = Cast<ADkCharacter>(GetOwner());
    }

    if (PlayerRef)
    {
        if (!PlayerControllerRef)
        {
            PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
        }

        PlayerCameraRef = Cast<UCameraComponent>(PlayerRef->GetComponentByClass(UCameraComponent::StaticClass()));
    }
    
    if (!PlayerControllerInterface)
    {
        PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
    }

    if(PlayerControllerInterface)
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

    if (!bIsExecutingScanning)
    {
        UDkScannableComponent* PreviousTarget = CurrentScannableTarget;
        InitiateSweepForScannables();
        
        if (PreviousTarget && PreviousTarget != CurrentScannableTarget)
        {
            PreviousTarget->UnhighlightAsTarget();
        }
        return;
    }

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
            if (CurrentScannableTarget)
            {
                for (UDkScannableComponent* Scannable : ScannablesInRange)
                {
                    if (Scannable)
                    {
                        Scannable->OnScanModeExited();
                    }
                }
                CurrentScannableTarget->UnhighlightAsTarget();
                CurrentScannableTarget = nullptr;
            }

        CurrentScannableTarget = nullptr;
    }
}

void UDkScanningComponent::OnScanExecuteStart()
{
    if (bIsInScanMode && CurrentScannableTarget && !bIsExecutingScanning)
    {
        bIsExecutingScanning = true;
        CurrentScannableTarget->OnScanStart();
        PrimaryComponentTick.bCanEverTick = true;
    }
}

void UDkScanningComponent::OnScanExecuteEnd()
{
    if (bIsExecutingScanning)
    {
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
    if (!PlayerCameraRef || !PlayerRef) { return false; }
  
    FVector CameraLocation = PlayerCameraRef->GetComponentLocation();
    FVector CameraForward = PlayerCameraRef->GetForwardVector();
    
    APlayerController* PC = Cast<APlayerController>(PlayerRef->GetController());
    if (!PC) { return false; }
  
    float CosMaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxAngleInDegrees));

    struct FScanScore 
    {
        UDkScannableComponent* Component;
        float Score;
  
        FScanScore(UDkScannableComponent* InComponent, float InScore)
            : Component(InComponent), Score(InScore) {}
    };

    TArray<FScanScore> ScoredScannables;
  
    for (UDkScannableComponent* Scannable : PotentialScannables)
    {
        if (!Scannable) continue;
        
        AActor* ScannableOwner = Scannable->GetOwner();
        if (!ScannableOwner) continue;

        FVector ScannableLocation = ScannableOwner->GetActorLocation();
        FVector2D ScreenPosition;
        
        if (PC->ProjectWorldLocationToScreen(ScannableLocation, ScreenPosition))
        {
            int32 ViewportSizeX, ViewportSizeY;
            PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
            FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

            FVector2D ScreenDelta = ScreenPosition - ScreenCenter;
            float ScreenDistance = ScreenDelta.Size() / FMath::Min(ViewportSizeX, ViewportSizeY);
            
            float ScreenScore = 1.0f - FMath::Min(ScreenDistance, 1.0f);
            ScoredScannables.Add(FScanScore(Scannable, ScreenScore));
        }
    }
  
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
        if (NewScannable != CurrentScannableTarget)
        {
            if (CurrentScannableTarget)
            {
                CurrentScannableTarget->UnhighlightAsTarget();
            }
            CurrentScannableTarget = NewScannable;
            CurrentScannableTarget->HighlightAsTarget();
        }
    }
    else if (CurrentScannableTarget)
    {
        CurrentScannableTarget->UnhighlightAsTarget();
        CurrentScannableTarget = nullptr;
    }
}
// Copyright @ Christian Reichel

#include "Component/DkScanningComponent.h"
#include "Component/DkScannableComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/DkCharacter.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DkPlayerController.h"
#include "Player/DkPlayerControllerInterface.h"

UDkScanningComponent::UDkScanningComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDkScanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!PlayerCameraRef || !PlayerControllerRef) return;

    // Handle scanning progress
    if (bIsExecutingScanning && CurrentScannableTarget)
    {
        float ScanDuration = CurrentScannableTarget->GetScanDuration();
        float CurrentProgress = CurrentScannableTarget->GetCurrentScanProgress();
        CurrentProgress += DeltaTime / ScanDuration;
        
        if (CurrentProgress >= 1.0f)
        {
            CurrentScannableTarget->OnScanComplete();
            bIsExecutingScanning = false;
            CurrentScannableTarget = nullptr;
            return;
        }
        
        CurrentScannableTarget->OnScanProgress(CurrentProgress);
        return; // Skip target detection while scanning
    }

    FVector Start = PlayerCameraRef->GetComponentLocation();
    FVector Forward = PlayerCameraRef->GetForwardVector();
    FVector End = Start + (Forward * TraceRange);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnFaceIndex = true;
    QueryParams.bReturnPhysicalMaterial = true;

    TArray<FHitResult> HitResults;
    bool bFoundScannable = false;

#if ENABLE_DRAW_DEBUG
    if (bShowDebugTraces)
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.0f);
    }
#endif

    if (GetWorld()->LineTraceMultiByObjectType(
        HitResults, 
        Start, 
        End, 
        FCollisionObjectQueryParams::AllObjects,
        QueryParams))
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (AActor* HitActor = Hit.GetActor())
            {
                if (UDkScannableComponent* NewTarget = HitActor->FindComponentByClass<UDkScannableComponent>())
                {
                    bFoundScannable = true;
                    if (NewTarget != CurrentScannableTarget)
                    {
                        if (CurrentScannableTarget)
                        {
                            CurrentScannableTarget->UnhighlightAsTarget();
                        }
                        CurrentScannableTarget = NewTarget;
                        CurrentScannableTarget->HighlightAsTarget();
                    }
                    break;
                }
            }
        }
    }

    if (!bFoundScannable && CurrentScannableTarget)
    {
        CurrentScannableTarget->UnhighlightAsTarget();
        CurrentScannableTarget = nullptr;
    }
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

void UDkScanningComponent::OnScanModeStart()
{
    if (!bIsInScanMode)
    {
        bIsInScanMode = true;

        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = false;
        QueryParams.AddIgnoredActor(GetOwner());

        FCollisionShape SphereShape;
        SphereShape.SetSphere(TraceRange);

        GetWorld()->OverlapMultiByChannel(
            OverlapResults,
            GetOwner()->GetActorLocation(),
            FQuat::Identity,
            ECollisionChannel::ECC_Visibility,
            SphereShape,
            QueryParams
        );

        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AActor* Actor = Result.GetActor())
            {
                if (UDkScannableComponent* Scannable = Actor->FindComponentByClass<UDkScannableComponent>())
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
        bIsInScanMode = false;
        
        if (bIsExecutingScanning)
        {
            OnScanExecuteEnd();
        }

        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = false;
        QueryParams.AddIgnoredActor(GetOwner());

        FCollisionShape SphereShape;
        SphereShape.SetSphere(TraceRange);

        GetWorld()->OverlapMultiByChannel(
            OverlapResults,
            GetOwner()->GetActorLocation(),
            FQuat::Identity,
            ECollisionChannel::ECC_Visibility,
            SphereShape,
            QueryParams
        );

        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AActor* Actor = Result.GetActor())
            {
                if (UDkScannableComponent* Scannable = Actor->FindComponentByClass<UDkScannableComponent>())
                {
                    Scannable->OnScanModeExited();
                }
            }
        }

        if (CurrentScannableTarget)
        {
            CurrentScannableTarget->UnhighlightAsTarget();
            CurrentScannableTarget = nullptr;
        }
    }
}

void UDkScanningComponent::OnScanExecuteStart()
{
    if (bIsInScanMode && CurrentScannableTarget && !bIsExecutingScanning)
    {
        bIsExecutingScanning = true;
        CurrentScannableTarget->OnScanStart();
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
    }
}
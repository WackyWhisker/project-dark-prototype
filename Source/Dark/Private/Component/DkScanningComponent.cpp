// Copyright @ Christian Reichel

#include "Component/DkScanningComponent.h"
#include "Component/DkScannableComponent.h"
#include "Component/DkFocusComponent.h"
#include "Character/DkCharacter.h"
#include "Player/DkPlayerController.h"
#include "Player/DkPlayerControllerInterface.h"

UDkScanningComponent::UDkScanningComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UDkScanningComponent::HasResource(EDkScanType ScanType, float Amount) const
{
    if (const FScanTypeValue* Value = ScannedValues.Find(ScanType))
    {
        return Value->CurrentValue >= Amount;
    }
    return false;
}

bool UDkScanningComponent::ConsumeResource(EDkScanType ScanType, float Amount)
{
    FScanTypeValue* Value = ScannedValues.Find(ScanType);
    if (Value && Value->CurrentValue >= Amount)
    {
        float OldValue = Value->CurrentValue;
        Value->CurrentValue -= Amount;
        OnScanValueChanged.Broadcast(ScanType, OldValue, Value->CurrentValue, Value->MaxValue);
        return true;
    }
    return false;
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
        PlayerControllerRef = Cast<ADkPlayerController>(PlayerRef->GetController());
    }
    
    if (PlayerControllerRef)
    {
        if (IDkPlayerControllerInterface* Interface = Cast<IDkPlayerControllerInterface>(PlayerControllerRef))
        {
            Interface->GetScanExecuteStartDelegate()->AddUObject(this, &UDkScanningComponent::HandleScanExecuteStart);
            Interface->GetScanExecuteEndDelegate()->AddUObject(this, &UDkScanningComponent::HandleScanExecuteEnd);
        }
    }
    
    // Get focus component reference
    FocusComponent = GetOwner()->FindComponentByClass<UDkFocusComponent>();
    if (FocusComponent)
    {
        FocusComponent->OnFocusStateChanged.AddUObject(this, &UDkScanningComponent::HandleFocusChanged);
        FocusComponent->OnFocusModeChanged.AddUObject(this, &UDkScanningComponent::HandleFocusModeChanged);
    }
    
    if (UWorld* World = GetWorld())
    {
        GameStateSubsystem = World->GetSubsystem<UDkGameStateSubsystem>();
        if (GameStateSubsystem)
        {
            GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UDkScanningComponent::HandleGameStateChanged);
        }
    }
}

void UDkScanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Only process scanning if active and focused
    if (!bIsScanningMode || !FocusComponent || !FocusComponent->IsFocused())
    {
        if (CurrentScannableTarget)
        {
            CurrentScannableTarget->UnhighlightAsTarget();
            CurrentScannableTarget = nullptr;
        }
        return;
    }

    // Handle scanning progress
    if (bIsExecutingScanning && CurrentScannableTarget)
    {
        float ScanDuration = CurrentScannableTarget->GetScanDuration();
        float CurrentProgress = CurrentScannableTarget->GetCurrentScanProgress();
        CurrentProgress += DeltaTime / ScanDuration;
        
        if (CurrentProgress >= 1.0f)
        {
            // Get the scan value and type
            EDkScanType ScanType = CurrentScannableTarget->GetScanType();
            float ScanValue = CurrentScannableTarget->GetScanValue();
            
            FScanTypeValue& TypeValue = ScannedValues.FindOrAdd(ScanType);
            float OldValue = TypeValue.CurrentValue;
            TypeValue.RetainOnDeath = CurrentScannableTarget->ShouldRetainOnDeath();
            TypeValue.MaxValue = CurrentScannableTarget->GetMaxValue();
            TypeValue.CurrentValue = FMath::Min(TypeValue.CurrentValue + ScanValue, TypeValue.MaxValue);

            OnScanValueChanged.Broadcast(ScanType, OldValue, TypeValue.CurrentValue, TypeValue.MaxValue);

            CurrentScannableTarget->OnScanComplete();
            bIsExecutingScanning = false;
            CurrentScannableTarget = nullptr;
            return;
        }
        
        CurrentScannableTarget->OnScanProgress(CurrentProgress);
        return; // Skip target detection while scanning
    }

    // Scanning raycast logic
    FVector Start = PlayerRef->GetActorLocation();
    FVector Forward = PlayerRef->GetActorForwardVector();
    FVector End = Start + (Forward * TraceRange);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnFaceIndex = true;
    QueryParams.bReturnPhysicalMaterial = true;

    TArray<FHitResult> HitResults;
    bool bFoundScannable = false;

    if (bShowDebugTraces)
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.0f);
    }

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

    // Notify Focus Component about target state
    if (FocusComponent)
    {
        FocusComponent->UpdateTargetWidget(bFoundScannable);
    }
}

void UDkScanningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (GameStateSubsystem)
    {
        GameStateSubsystem->OnGameStateChanged.RemoveDynamic(this, &UDkScanningComponent::HandleGameStateChanged);
    }
  
    if (PlayerControllerRef)
    {
        if (IDkPlayerControllerInterface* Interface = Cast<IDkPlayerControllerInterface>(PlayerControllerRef))
        {
            Interface->GetScanExecuteStartDelegate()->RemoveAll(this);
            Interface->GetScanExecuteEndDelegate()->RemoveAll(this);
        }
    }

    if (FocusComponent)
    {
        FocusComponent->OnFocusStateChanged.RemoveAll(this);
        FocusComponent->OnFocusModeChanged.RemoveAll(this);
    }
}

void UDkScanningComponent::HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
    if (NewState == EDkGameState::Dying || NewState == EDkGameState::Retreat)
    {
        ResetValuesOnDeath();
    }
}

void UDkScanningComponent::ResetValuesOnDeath()
{
    for (auto& Pair : ScannedValues)
    {
        if (!Pair.Value.RetainOnDeath)
        {
            float OldValue = Pair.Value.CurrentValue;
            Pair.Value.CurrentValue = 0;
            OnScanValueChanged.Broadcast(Pair.Key, OldValue, Pair.Value.CurrentValue, Pair.Value.MaxValue);
        }
    }
}

void UDkScanningComponent::HandleFocusChanged(bool bIsFocused)
{
    if (!bIsFocused)
    {
        if (bIsExecutingScanning)
        {
            HandleScanExecuteEnd();
        }

        if (CurrentScannableTarget)
        {
            CurrentScannableTarget->UnhighlightAsTarget();
            CurrentScannableTarget = nullptr;
        }
    }
}

void UDkScanningComponent::HandleFocusModeChanged(EDkFocusMode NewMode, EDkFocusMode OldMode)
{
    bool bWasActive = bIsScanningMode;
    bIsScanningMode = (NewMode == EDkFocusMode::Scanning);
    
    if (!bIsScanningMode && bWasActive)
    {
        HandleFocusChanged(false);
    }
}

void UDkScanningComponent::HandleScanExecuteStart()
{
    if (!bIsScanningMode || !FocusComponent || !FocusComponent->IsFocused())
        return;
        
    if (CurrentScannableTarget && !bIsExecutingScanning)
    {
        // Get current value for this type
        EDkScanType ScanType = CurrentScannableTarget->GetScanType();
        FScanTypeValue& TypeValue = ScannedValues.FindOrAdd(ScanType);
        
        // Only block if we're already at max
        if (TypeValue.CurrentValue < TypeValue.MaxValue)
        {
            bIsExecutingScanning = true;
            CurrentScannableTarget->OnScanStart();
        }
    }
}

void UDkScanningComponent::HandleScanExecuteEnd()
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
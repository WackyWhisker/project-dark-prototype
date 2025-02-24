// Copyright @ Christian Reichel

#include "Component/DkScanningComponent.h"
#include "Component/DkScannableComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/DkCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
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
        // Get the camera boom (usually named CameraBoom in the character blueprint)
        CameraBoomRef = PlayerRef->FindComponentByClass<USpringArmComponent>();
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

    if (PlayerCameraRef)
    {
        OriginalCameraLocation = PlayerCameraRef->GetRelativeLocation();
    }
    
    // Get game state subsystem
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

    // Debug HUD - always show values
    /*if (GEngine)
    {
        int32 Line = 0;
        for (auto It = ScannedValues.CreateConstIterator(); It; ++It)
        {
            FString TypeName = UEnum::GetValueAsString(It->Key);
            FString DebugText = FString::Printf(TEXT("%s: %d (Retain: %s)"), 
                *TypeName, 
                It->Value.CurrentValue,
                It->Value.RetainOnDeath ? TEXT("Yes") : TEXT("No"));
                
            GEngine->AddOnScreenDebugMessage(
                -10000 - Line,  // Using -10000 as base to avoid conflict with other debug messages
                0.0f,          
                FColor::Cyan,
                DebugText
            );
            Line++;
        }
    }*/
    
    if (!PlayerCameraRef || !PlayerControllerRef) return;

    float TargetOffset = bIsInScanMode ? CameraScanOffset : 0.0f;
    CurrentCameraOffset = FMath::FInterpTo(CurrentCameraOffset, TargetOffset, DeltaTime, CameraInterpolationSpeed);
    
    // Get world-space right vector
    FRotator ControlRotation = PlayerControllerRef->GetControlRotation();
    FVector WorldRightOffset = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y) * CurrentCameraOffset;
    
    // Convert to local space
    FVector LocalOffset = PlayerCameraRef->GetComponentTransform().InverseTransformVector(WorldRightOffset);
    FVector NewLocation = OriginalCameraLocation + LocalOffset;
    
    PlayerCameraRef->SetRelativeLocation(NewLocation);

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
    if (bIsInScanMode)  // Only process widget changes when in scan mode
    {
        if (bFoundScannable)
        {
            if (CrosshairWidgetTargetClass && (!CrosshairWidget || CrosshairWidget->GetClass() != CrosshairWidgetTargetClass))
            {
                if (CrosshairWidget)
                {
                    CrosshairWidget->RemoveFromParent();
                }
                CrosshairWidget = CreateWidget<UUserWidget>(PlayerControllerRef, CrosshairWidgetTargetClass);
                CrosshairWidget->AddToViewport();
            }
        }
        else if (CrosshairWidgetClass && (!CrosshairWidget || CrosshairWidget->GetClass() != CrosshairWidgetClass))
        {
            if (CrosshairWidget)
            {
                CrosshairWidget->RemoveFromParent();
            }
            CrosshairWidget = CreateWidget<UUserWidget>(PlayerControllerRef, CrosshairWidgetClass);
            CrosshairWidget->AddToViewport();
        }
    }
}

void UDkScanningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (GameStateSubsystem)
    {
        GameStateSubsystem->OnGameStateChanged.RemoveDynamic(this, &UDkScanningComponent::HandleGameStateChanged);
    }
  
    if (PlayerControllerInterface)
    {
        PlayerControllerInterface->GetScanModeStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanModeEndDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanExecuteStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetScanExecuteEndDelegate()->RemoveAll(this);
    }
}

void UDkScanningComponent::HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
    if (NewState == EDkGameState::Dying || NewState == EDkGameState::Retreat) //TODO: I think I need to not do this on retreat.
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
            
            // Broadcast the value change to notify listeners
            OnScanValueChanged.Broadcast(Pair.Key, OldValue, Pair.Value.CurrentValue, Pair.Value.MaxValue);
        }
    }
}

void UDkScanningComponent::OnScanModeStart()
{
    if (!bIsInScanMode)
    {
        bIsInScanMode = true;

        // Add crosshair widget
        if (CrosshairWidgetClass && !CrosshairWidget && PlayerControllerRef)
        {
            CrosshairWidget = CreateWidget<UUserWidget>(PlayerControllerRef, CrosshairWidgetClass);
            if (CrosshairWidget)
            {
                CrosshairWidget->AddToViewport();
            }
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
        
        // Remove crosshair widget
        if (CrosshairWidget)
        {
            CrosshairWidget->RemoveFromParent();
            CrosshairWidget = nullptr;
        }

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
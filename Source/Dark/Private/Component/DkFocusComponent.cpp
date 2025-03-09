// Copyright @ Christian Reichel

#include "Component/DkFocusComponent.h"
#include "Player/DkPlayerControllerInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "Enemy/DkEnemyBase.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Character/DkCharacter.h"

UDkFocusComponent::UDkFocusComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsFocused = false;
    bUseFocusToggle = true;
    CurrentMode = EDkFocusMode::Firearm;
    CurrentCameraOffset = 0.0f;
    CurrentArmLength = 0.0f;
}

void UDkFocusComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get camera references
   OwnerPlayerCharacter = Cast<ADkCharacter>(GetOwner());
    if (OwnerPlayerCharacter)
    {
        PlayerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
        CameraBoom = Cast<USpringArmComponent>(GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass()));
        
        if (PlayerCamera)
        {
            OriginalCameraLocation = PlayerCamera->GetRelativeLocation();
        }
        
        if (CameraBoom)
        {
            CurrentArmLength = CameraBoom->TargetArmLength;
        }

        if (APlayerController* PC = Cast<APlayerController>(OwnerPlayerCharacter->GetController()))
        {
            PlayerControllerInterface = Cast<IDkPlayerControllerInterface>(PC);
            if (PlayerControllerInterface)
            {
                PlayerControllerInterface->GetFocusStartDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusStart);
                PlayerControllerInterface->GetFocusEndDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusEnd);
                PlayerControllerInterface->GetSwitchFocusModeDelegate()->AddUObject(this, &UDkFocusComponent::HandleFocusModeSwitch);
            }
        }
    }
}

void UDkFocusComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (PlayerControllerInterface)
    {
        PlayerControllerInterface->GetFocusStartDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetFocusEndDelegate()->RemoveAll(this);
        PlayerControllerInterface->GetSwitchFocusModeDelegate()->RemoveAll(this);
    }

    // Unhighlight enemy if one is targeted
    if (CurrentEnemyTarget)
    {
        CurrentEnemyTarget->UnhighlightAsTarget();
        CurrentEnemyTarget = nullptr;
    }

    ClearWidget();
}

void UDkFocusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!PlayerCamera || !CameraBoom) return;
    
    UpdateCamera(DeltaTime);
    
    // Always detect enemy targets regardless of focus mode
    DetectEnemyTarget();
}

void UDkFocusComponent::DetectEnemyTarget()
{
    if (!PlayerCamera) return;
    
    FVector Start = PlayerCamera->GetComponentLocation();
    FVector Forward = PlayerCamera->GetForwardVector();
    FVector End = Start + (Forward * EnemyTraceRange);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;
    
    if (bShowEnemyDebugTraces)
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.0f, 0, 1.0f);
    }
    
    TArray<FHitResult> HitResults;
    bool bFoundEnemy = false;
    
    if (GetWorld()->LineTraceMultiByChannel(
        HitResults,
        Start,
        End,
        ECC_Visibility,
        QueryParams))
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (AActor* HitActor = Hit.GetActor())
            {
                ADkEnemyBase* Enemy = Cast<ADkEnemyBase>(HitActor);
                if (Enemy)
                {
                    // Check for collision with the detection sphere
                    USphereComponent* DetectionSphere = Enemy->DetectionSphere;
                    if (DetectionSphere && DetectionSphere->IsCollisionEnabled())
                    {
                        bFoundEnemy = true;
                        
                        if (Enemy != CurrentEnemyTarget)
                        {
                            // Unhighlight previous target
                            if (CurrentEnemyTarget)
                            {
                                CurrentEnemyTarget->UnhighlightAsTarget();
                            }
                            
                            // Highlight new target
                            CurrentEnemyTarget = Enemy;
                            CurrentEnemyTarget->HighlightAsTarget();
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // If no enemy found but we had one highlighted, unhighlight it
    if (!bFoundEnemy && CurrentEnemyTarget)
    {
        CurrentEnemyTarget->UnhighlightAsTarget();
        CurrentEnemyTarget = nullptr;
    }
}

void UDkFocusComponent::UpdateCamera(float DeltaTime)
{
    // Update camera offset
    float TargetOffset = GetTargetCameraOffset();
    CurrentCameraOffset = FMath::FInterpTo(CurrentCameraOffset, TargetOffset, DeltaTime, CameraInterpolationSpeed);
    
    // Get world-space right vector and apply offset
    APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
    if (PC)
    {
        FRotator ControlRotation = PC->GetControlRotation();
        FVector WorldRightOffset = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y) * CurrentCameraOffset;
        FVector LocalOffset = PlayerCamera->GetComponentTransform().InverseTransformVector(WorldRightOffset);
        PlayerCamera->SetRelativeLocation(OriginalCameraLocation + LocalOffset);
    }
    
    // Update spring arm length
    float TargetArmLength = GetTargetArmLength();
    CurrentArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, CameraInterpolationSpeed);
    CameraBoom->TargetArmLength = CurrentArmLength;
}

float UDkFocusComponent::GetTargetCameraOffset() const
{
    if (!bIsFocused) return 0.0f;
    
    switch (CurrentMode)
    {
        case EDkFocusMode::Scanning:
            return ScanningCameraOffset;
        case EDkFocusMode::Firearm:
            return FirearmCameraOffset;
        default:
            return 0.0f;
    }
}

float UDkFocusComponent::GetTargetArmLength() const
{
    if (!bIsFocused) return DefaultArmLength;
    
    switch (CurrentMode)
    {
        case EDkFocusMode::Scanning:
            return ScanningArmLength;
        case EDkFocusMode::Firearm:
            return FirearmArmLength;
        default:
            return DefaultArmLength;
    }
}

void UDkFocusComponent::UpdateTargetWidget(bool bHasTarget)
{
    if (!PlayerControllerInterface) return;
    
    TSubclassOf<UUserWidget> TargetWidgetClass = nullptr;
    
    // Determine which widget to show based on mode and target status
    switch (CurrentMode)
    {
        case EDkFocusMode::Scanning:
            TargetWidgetClass = bHasTarget ? ScanningTargetCrosshairClass : ScanningCrosshairClass;
            break;
        case EDkFocusMode::Firearm:
            TargetWidgetClass = bHasTarget ? FirearmTargetCrosshairClass : FirearmCrosshairClass;
            break;
        default:
            break;
    }
    
    // Only update if we need to change widget
    if (TargetWidgetClass && (!CurrentWidget || CurrentWidget->GetClass() != TargetWidgetClass))
    {
        ClearWidget();
        
        APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
        if (PC)
        {
            CurrentWidget = CreateWidget<UUserWidget>(PC, TargetWidgetClass);
            if (CurrentWidget)
            {
                CurrentWidget->AddToViewport();
            }
        }
    }
}

void UDkFocusComponent::ClearWidget()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }
}

void UDkFocusComponent::HandleFocusStart()
{
    bIsFocused = true;
    UpdateTargetWidget(false);
    OnFocusModeChanged.Broadcast(CurrentMode, EDkFocusMode::None);
    OnFocusStateChanged.Broadcast(true);
    if (OwnerPlayerCharacter)
    {
        OwnerPlayerCharacter->DkPlayerAimingAnimationState = 
    (CurrentMode == EDkFocusMode::Firearm) ? 
    Firearm : 
    Scanning;
    }
}

void UDkFocusComponent::HandleFocusEnd()
{
    bIsFocused = false;
    ClearWidget();
    OnFocusStateChanged.Broadcast(false);
    if (OwnerPlayerCharacter)
    {
        OwnerPlayerCharacter->DkPlayerAimingAnimationState = EDkPlayerAimingAnimationState::None;
    }
}

void UDkFocusComponent::HandleFocusModeSwitch()
{
    if (!bIsFocused)
        return;
        
    EDkFocusMode NewMode = (CurrentMode == EDkFocusMode::Firearm) ? 
        EDkFocusMode::Scanning : EDkFocusMode::Firearm;
        
    EDkFocusMode OldMode = CurrentMode;
    CurrentMode = NewMode;
    
    UpdateTargetWidget(false);
    OnFocusModeChanged.Broadcast(NewMode, OldMode);

    if (OwnerPlayerCharacter)
    {
        OwnerPlayerCharacter->DkPlayerAimingAnimationState = 
    (CurrentMode == EDkFocusMode::Firearm) ? 
    Firearm : 
    Scanning;
    }
}

void UDkFocusComponent::SetFocusToggleMode(bool bNewToggleMode)
{
    if (bUseFocusToggle == bNewToggleMode)
        return;
        
    bUseFocusToggle = bNewToggleMode;
    
    if (!bUseFocusToggle && bIsFocused)
    {
        HandleFocusEnd();
    }
}
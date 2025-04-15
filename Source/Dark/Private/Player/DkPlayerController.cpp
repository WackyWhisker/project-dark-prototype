// Copyright @ Christian Reichel

#include "Player/DkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/DkCharacter.h"

DEFINE_LOG_CATEGORY(LogDkPlayerController);

// Base controller methods
void ADkPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
    bIsFocusModeShooting = true;
    
    if (!PlayerRef)
    {
        PlayerRef = Cast<ADkCharacter>(GetPawn());
        UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerRef set"));
    }
    if (PlayerRef)
    {
        PlayerSpringArmRef = PlayerRef->FindComponentByClass<USpringArmComponent>();
    }

    //Targeting UI
    if (LetterboxWidgetClass)
    {
        LetterboxWidget = CreateWidget<UUserWidget>(this, LetterboxWidgetClass);
    }

    //Subscribe to Castle Game State broadcasts
    if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
    {
        GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &ADkPlayerController::HandleGameStateChanged);
    }
}

void ADkPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
    
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADkPlayerController::Look);

        //Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADkPlayerController::Jump);

        //Dodge
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ADkPlayerController::Dodge);

        //Attacking
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADkPlayerController::Attack);

        //Interact
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADkPlayerController::Interact);

        EnhancedInputComponent->BindAction(SwitchFocusModeAction, ETriggerEvent::Started, this, &ADkPlayerController::SwitchFocusMode);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ADkPlayerController::Shoot);
        EnhancedInputComponent->BindAction(ScanExecuteAction, ETriggerEvent::Started, this, &ADkPlayerController::ScanExecuteStart);
        EnhancedInputComponent->BindAction(ScanExecuteAction, ETriggerEvent::Completed, this, &ADkPlayerController::ScanExecuteEnd);
        
        // Store enhanced input component for rebinding
        CachedEnhancedInputComponent = EnhancedInputComponent;
        
        // Setup initial targeting and focus bindings
        SetupTargetingBindings();
        SetupFocusBindings();

        //Drop and Lift
        EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &ADkPlayerController::Drop);
        EnhancedInputComponent->BindAction(LiftAction, ETriggerEvent::Started, this, &ADkPlayerController::Lift);

        //Pause Menu
        EnhancedInputComponent->BindAction(TogglePauseMenuAction, ETriggerEvent::Started, this, &ADkPlayerController::TogglePauseMenu);

        //Upgrade Menu
        EnhancedInputComponent->BindAction(ToggleUpgradeMenuAction, ETriggerEvent::Started, this, &ADkPlayerController::ToggleUpgradeMenu);
    }
    else
    {
        UE_LOG(LogDkPlayerController, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
    }
}

// Input handling methods
void ADkPlayerController::Look(const FInputActionValue& Value)
{
    if (!PlayerRef) {return;}
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    
    PlayerRef->AddControllerYawInput(LookAxisVector.X * TargetingYawInputScale);
    PlayerRef->AddControllerPitchInput(LookAxisVector.Y);
}

// Basic player actions
void ADkPlayerController::Jump()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Jump Input"));
    if (JumpDelegate.IsBound())
    {
        JumpDelegate.Broadcast();
    }
}

void ADkPlayerController::Dodge()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Dodge Input"));
    if (DodgeDelegate.IsBound())
    {
        DodgeDelegate.Broadcast();
    }
}

void ADkPlayerController::Attack()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Attack Input"));
    if (AttackDelegate.IsBound())
    {
        AttackDelegate.Broadcast();
    }
}

void ADkPlayerController::Interact()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Attack Input"));
    if (InteractDelegate.IsBound())
    {
        InteractDelegate.Broadcast();
    }
}

void ADkPlayerController::Drop()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Drop Input"));
    if (DropDelegate.IsBound())
    {
        DropDelegate.Broadcast();
    }
}

void ADkPlayerController::Lift()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Lift Input"));
    if (LiftDelegate.IsBound())
    {
        LiftDelegate.Broadcast();
    }
}

// Menu controls
void ADkPlayerController::TogglePauseMenu()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Pause Menu Input"));
    if (TogglePauseMenuDelegate.IsBound())
    {
        TogglePauseMenuDelegate.Broadcast();
    }
}

void ADkPlayerController::ToggleUpgradeMenu()
{
    UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerController: Upgrade Menu Input"));
    if (ToggleUpgradeMenuDelegate.IsBound())
    {
        ToggleUpgradeMenuDelegate.Broadcast();
    }
}

// UI and mapping context
void ADkPlayerController::ToggleLetterboxUI(bool bShowLetterboxUI)
{
    if (!LetterboxWidget) {return;}

    if (bShowLetterboxUI)
    {
        if (!LetterboxWidget->IsInViewport())
        {
            LetterboxWidget->AddToViewport();
        }
    }
    else
    {
        if(LetterboxWidget->IsInViewport())
        {
            LetterboxWidget->RemoveFromParent();
        }
    }
}

void ADkPlayerController::SetMappingContext(const FName& ContextName, bool bEnable)
{
    if (UInputMappingContext* Context = MappingContexts.FindRef(ContextName))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            if (bEnable)
            {
                Subsystem->AddMappingContext(Context, 0);
            }
            else
            {
                Subsystem->RemoveMappingContext(Context);
            }
        }
    }
}

// Targeting system
void ADkPlayerController::TargetStart()
{
    if (!bIsTargeting || !bUseTargetModeToggle)
    {
        if (TargetStartDelegate.IsBound())
        {
            TargetStartDelegate.Broadcast();
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(TargetMappingContext, 0);
            }
        }
        if (!bUseTargetModeToggle)
        {
            bIsTargeting = true;
        }
    }
}

void ADkPlayerController::TargetEnd()
{
    if (bIsTargeting || !bUseTargetModeToggle)
    {
        if (TargetEndDelegate.IsBound())
        {
            TargetEndDelegate.Broadcast();
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
            {
                Subsystem->RemoveMappingContext(TargetMappingContext);
            }
        }
        if (!bUseTargetModeToggle)
        {
            bIsTargeting = false;
        }
    }
}

void ADkPlayerController::TargetToggle()
{
    if (!bIsTargeting)
    {
        TargetStart();
    }
    else
    {
        TargetEnd();
    }
    bIsTargeting = !bIsTargeting;
}

void ADkPlayerController::ToggleTargetMode()
{
    bUseTargetModeToggle = !bUseTargetModeToggle;
    SetupTargetingBindings();
}

void ADkPlayerController::SetTargetingMode(bool bNewToggleMode)
{
    if (bUseTargetModeToggle == bNewToggleMode) return;
    
    bUseTargetModeToggle = bNewToggleMode;
    
    if (!bUseTargetModeToggle && bIsTargeting)
    {
        TargetEnd();
        bIsTargeting = false;
    }
    
    SetupTargetingBindings();
}

void ADkPlayerController::TargetCycleLeft()
{
    if(TargetCycleLeftDelegate.IsBound())
    {
        TargetCycleLeftDelegate.Broadcast();
    }
}

void ADkPlayerController::TargetCycleRight()
{
    if(TargetCycleRightDelegate.IsBound())
    {
        TargetCycleRightDelegate.Broadcast();
    }
}

void ADkPlayerController::ResetTargetingState()
{
    bIsTargeting = false;
    
    if (bUseTargetModeToggle)
    {
        SetupTargetingBindings();
    }
}

void ADkPlayerController::SetupTargetingBindings()
{
    if (!CachedEnhancedInputComponent) return;
    
    // Clear existing targeting bindings if any
    CachedEnhancedInputComponent->RemoveBindingByHandle(TargetStartHandle);
    CachedEnhancedInputComponent->RemoveBindingByHandle(TargetEndHandle);
    
    // Set up main targeting binding based on mode
    if (bUseTargetModeToggle)
    {
        TargetStartHandle = CachedEnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ADkPlayerController::TargetToggle).GetHandle();
    }
    else
    {
        TargetStartHandle = CachedEnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ADkPlayerController::TargetStart).GetHandle();
        TargetEndHandle = CachedEnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Completed, this, &ADkPlayerController::TargetEnd).GetHandle();
    }

    // Always bind cycling actions regardless of mode
    CachedEnhancedInputComponent->BindAction(TargetCycleLeftAction, ETriggerEvent::Started, this, &ADkPlayerController::TargetCycleLeft);
    CachedEnhancedInputComponent->BindAction(TargetCycleRightAction, ETriggerEvent::Started, this, &ADkPlayerController::TargetCycleRight);
}

// Focus system
void ADkPlayerController::FocusStart()
{
    if (!bIsFocused || !bUseFocusToggle)
    {
        bIsFocused = true;
        
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(FocusMappingContext, 1);
        }
        
        ApplyCurrentFocusMode();
        
        if (FocusStartDelegate.IsBound())
        {
            FocusStartDelegate.Broadcast();
        }
    }
}

void ADkPlayerController::FocusEnd()
{
    if (bIsFocused || !bUseFocusToggle)
    {
        bIsFocused = false;
        
        // End any active scanning
        ScanExecuteEnd();
        
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(FocusMappingContext);
            Subsystem->RemoveMappingContext(ScanningMappingContext);
            Subsystem->RemoveMappingContext(ShootingMappingContext);
        }
        
        if (FocusEndDelegate.IsBound())
        {
            FocusEndDelegate.Broadcast();
        }
    }
}

void ADkPlayerController::FocusToggle()
{
    if (!bIsFocused)
    {
        FocusStart();
    }
    else
    {
        FocusEnd();
    }
}

void ADkPlayerController::SetFocusToggleMode(bool bNewToggleMode)
{
    if (bUseFocusToggle == bNewToggleMode) return;
    
    bUseFocusToggle = bNewToggleMode;
    
    if (!bUseFocusToggle && bIsFocused)
    {
        FocusEnd();
    }
    
    SetupFocusBindings();
}

void ADkPlayerController::ToggleFocusMode()
{
    bUseFocusToggle = !bUseFocusToggle;
    SetupFocusBindings();
}

void ADkPlayerController::SwitchFocusMode()
{
    // Toggle between shooting and scanning modes
    bIsFocusModeShooting = !bIsFocusModeShooting;
    
    // Apply the new focus mode if we're currently focused
    if (bIsFocused)
    {
        ApplyCurrentFocusMode();
    }
    
    if (SwitchFocusModeDelegate.IsBound())
    {
        SwitchFocusModeDelegate.Broadcast();
    }
}

void ADkPlayerController::Shoot()
{
    // Only broadcast if we're focused and in shooting mode
    if (bIsFocused && bIsFocusModeShooting && ShootDelegate.IsBound())
    {
        ShootDelegate.Broadcast();
    }
}

void ADkPlayerController::ScanExecuteStart()
{
    // Only broadcast if we're focused and in scanning mode
    if (bIsFocused && !bIsFocusModeShooting && ScanExecuteStartDelegate.IsBound())
    {
        ScanExecuteStartDelegate.Broadcast();
    }
}

void ADkPlayerController::ScanExecuteEnd()
{
    // Only broadcast if we're focused and in scanning mode
    // Note: We might want to always broadcast end regardless of mode
    if ((!bIsFocused || !bIsFocusModeShooting) && ScanExecuteEndDelegate.IsBound())
    {
        ScanExecuteEndDelegate.Broadcast();
    }
}

void ADkPlayerController::SetupFocusBindings()
{
    if (!CachedEnhancedInputComponent) return;
    
    // Clear existing bindings if any
    CachedEnhancedInputComponent->RemoveBindingByHandle(FocusStartHandle);
    CachedEnhancedInputComponent->RemoveBindingByHandle(FocusEndHandle);
    
    // Set up main focus binding based on mode
    if (bUseFocusToggle)
    {
        FocusStartHandle = CachedEnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Started, this, &ADkPlayerController::FocusToggle).GetHandle();
    }
    else
    {
        FocusStartHandle = CachedEnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Started, this, &ADkPlayerController::FocusStart).GetHandle();
        FocusEndHandle = CachedEnhancedInputComponent->BindAction(FocusAction, ETriggerEvent::Completed, this, &ADkPlayerController::FocusEnd).GetHandle();
    }
}

// Interface method implementations
FJumpSignature* ADkPlayerController::GetJumpDelegate()
{
    return &JumpDelegate;
}

FDodgeSignature* ADkPlayerController::GetDodgeDelegate()
{
    return &DodgeDelegate;
}

FAttackSignature* ADkPlayerController::GetAttackDelegate()
{
    return &AttackDelegate;
}

FInteractSignature* ADkPlayerController::GetInteractDelegate()
{
    return &InteractDelegate;
}

FTargetStartSignature* ADkPlayerController::GetTargetStartDelegate()
{
    return &TargetStartDelegate;
}

FTargetEndSignature* ADkPlayerController::GetTargetEndDelegate()
{
    return &TargetEndDelegate;
}

FTargetCycleLeftSignature* ADkPlayerController::GetTargetCycleLeftDelegate()
{
    return &TargetCycleLeftDelegate;
}

FTargetCycleRightSignature* ADkPlayerController::GetTargetCycleRightDelegate()
{
    return &TargetCycleRightDelegate;
}

FDropSignature* ADkPlayerController::GetDropDelegate()
{
    return &DropDelegate;
}

FLiftSignature* ADkPlayerController::GetLiftDelegate()
{
    return &LiftDelegate;
}

FTogglePauseMenuSignature* ADkPlayerController::GetTogglePauseMenuDelegate()
{
    return &TogglePauseMenuDelegate;
}

FToggleUpgradeMenuSignature* ADkPlayerController::GetToggleUpgradeMenuDelegate()
{
    return &ToggleUpgradeMenuDelegate;
}

FScanExecuteStartSignature* ADkPlayerController::GetScanExecuteStartDelegate()
{
    return &ScanExecuteStartDelegate;
}

FScanExecuteEndSignature* ADkPlayerController::GetScanExecuteEndDelegate()
{
    return &ScanExecuteEndDelegate;
}

FShootSignature* ADkPlayerController::GetShootDelegate()
{
    return &ShootDelegate;
}

FFocusStartSignature* ADkPlayerController::GetFocusStartDelegate()
{
    return &FocusStartDelegate;
}

FFocusEndSignature* ADkPlayerController::GetFocusEndDelegate()
{
    return &FocusEndDelegate;
}

FSwitchFocusModeSignature* ADkPlayerController::GetSwitchFocusModeDelegate()
{
    return &SwitchFocusModeDelegate;
}

// Game state handling and sequences
void ADkPlayerController::HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
    switch (NewState)
    {
    case EDkGameState::Dying:
        // Disable input
            DisableInput(this);
        bWasInputDisabledByDeath = true;
        PlayDeathSequence();
        break;

    case EDkGameState::Retreat:
        DisableInput(this);
        bWasInputDisabledByDeath = true;
        PlayRetreatSequence();
        break;

    case EDkGameState::Respawning:
        PlayRespawnSequence();
        break;
            
    case EDkGameState::Playing:
        // Only re-enable if we were the ones who disabled it
            if (bWasInputDisabledByDeath)
            {
                EnableInput(this);
                bWasInputDisabledByDeath = false;
            }
        break;
    }
}

void ADkPlayerController::PlayDeathSequence()
{
   if (!DeathSequence)
   {
       UE_LOG(LogDkPlayerController, Warning, TEXT("Death sequence not set in PlayerController"));
       if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
       {
           GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
       }
       return;
   }

   // Create sequence player and bind to finish event
   FMovieSceneSequencePlaybackSettings PlaybackSettings;
   PlaybackSettings.bPauseAtEnd = false;
   ALevelSequenceActor* LSActor;  // Will store the created actor
   
   ActiveSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
       GetWorld(),
       DeathSequence,
       PlaybackSettings,
       LSActor  // Pass the actor reference
   );

   if (ActiveSequencePlayer)
   {
       ActiveSequencePlayer->OnFinished.AddDynamic(this, &ADkPlayerController::OnDeathSequenceFinished);
       ActiveSequencePlayer->Play();
   }
   else
   {
       UE_LOG(LogDkPlayerController, Warning, TEXT("Failed to create death sequence player"));
       if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
       {
           GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
       }
   }
}

void ADkPlayerController::PlayRespawnSequence()
{
   if (!RespawnSequence)
   {
       UE_LOG(LogDkPlayerController, Warning, TEXT("Respawn sequence not set in PlayerController"));
       if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
       {
           GameStateSubsystem->RequestStateChange(EDkGameState::Playing);
       }
       return;
   }

   // Create sequence player and bind to finish event
   FMovieSceneSequencePlaybackSettings PlaybackSettings;
   PlaybackSettings.bPauseAtEnd = false;
   ALevelSequenceActor* LSActor;  // Will store the created actor
   
   ActiveSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
       GetWorld(),
       RespawnSequence,
       PlaybackSettings,
       LSActor  // Pass the actor reference
   );

   if (ActiveSequencePlayer)
   {
       ActiveSequencePlayer->OnFinished.AddDynamic(this, &ADkPlayerController::OnRespawnSequenceFinished);
       ActiveSequencePlayer->Play();
   }
   else
   {
       UE_LOG(LogDkPlayerController, Warning, TEXT("Failed to create respawn sequence player"));
       if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
       {
           GameStateSubsystem->RequestStateChange(EDkGameState::Playing);
       }
   }
}

void ADkPlayerController::PlayRetreatSequence()
{
    if (!RetreatSequence)
    {
        UE_LOG(LogDkPlayerController, Warning, TEXT("Retreat sequence not set in PlayerController"));
        if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
        {
            GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
        }
        return;
    }

    // Create sequence player and bind to finish event
    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    PlaybackSettings.bPauseAtEnd = false;
    ALevelSequenceActor* LSActor;  // Will store the created actor
   
    ActiveSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        GetWorld(),
        RetreatSequence,
        PlaybackSettings,
        LSActor
    );

    if (ActiveSequencePlayer)
    {
        ActiveSequencePlayer->OnFinished.AddDynamic(this, &ADkPlayerController::OnRetreatSequenceFinished);
        ActiveSequencePlayer->Play();
    }
    else
    {
        UE_LOG(LogDkPlayerController, Warning, TEXT("Failed to create retreat sequence player"));
        if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
        {
            GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
        }
    }
}



void ADkPlayerController::OnDeathSequenceFinished()
{
   // Clean up
   if (ActiveSequencePlayer)
   {
       ActiveSequencePlayer->OnFinished.RemoveDynamic(this, &ADkPlayerController::OnDeathSequenceFinished);
       ActiveSequencePlayer = nullptr;
   }

   // Proceed to next state
   if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
   {
       GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
   }
}

void ADkPlayerController::OnRespawnSequenceFinished()
{
   // Clean up
   if (ActiveSequencePlayer)
   {
       ActiveSequencePlayer->OnFinished.RemoveDynamic(this, &ADkPlayerController::OnRespawnSequenceFinished);
       ActiveSequencePlayer = nullptr;
   }

   // Proceed to next state
   if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
   {
       GameStateSubsystem->RequestStateChange(EDkGameState::Playing);
   }
}

void ADkPlayerController::OnRetreatSequenceFinished()
{
    // Clean up
    if (ActiveSequencePlayer)
    {
        ActiveSequencePlayer->OnFinished.RemoveDynamic(this, &ADkPlayerController::OnRetreatSequenceFinished);
        ActiveSequencePlayer = nullptr;
    }

    // Proceed to next state
    if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
    {
        GameStateSubsystem->RequestStateChange(EDkGameState::Resetting);
    }
}


void ADkPlayerController::ApplyCurrentFocusMode()
{
    if (!bIsFocused)
    {
        return;
    }
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // Remove both contexts first
        Subsystem->RemoveMappingContext(ScanningMappingContext);
        Subsystem->RemoveMappingContext(ShootingMappingContext);
        
        // Add appropriate context
        if (bIsFocusModeShooting)
        {
            Subsystem->AddMappingContext(ShootingMappingContext, 2);
        }
        else
        {
            Subsystem->AddMappingContext(ScanningMappingContext, 2);
        }
    }
}
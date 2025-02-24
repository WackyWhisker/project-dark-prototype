// Copyright @ Christian Reichel

#include "Player/DkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"

#include "Character/DkCharacter.h"

DEFINE_LOG_CATEGORY(LogDkPlayerController);

void ADkPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
    
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
        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADkPlayerController::Move);

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
        
        // Store enhanced input component for rebinding
        CachedEnhancedInputComponent = EnhancedInputComponent;
        
        // Setup initial targeting and scanning bindings
        SetupTargetingBindings();
        SetupScanBindings();

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

void ADkPlayerController::ToggleTargetMode()
{
    bUseTargetModeToggle = !bUseTargetModeToggle;
    SetupTargetingBindings();
    
    FString Message = FString::Printf(TEXT("Target Mode: %s"), bUseTargetModeToggle ? TEXT("Toggle") : TEXT("Hold"));
    ClientMessage(Message);
}

void ADkPlayerController::ToggleScanMode()
{
    bUseScanModeToggle = !bUseScanModeToggle;
    SetupScanBindings();
    
    FString Message = FString::Printf(TEXT("Scan Mode: %s"), bUseScanModeToggle ? TEXT("Toggle") : TEXT("Hold"));
    ClientMessage(Message);
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

void ADkPlayerController::SetupScanBindings()
{
    if (!CachedEnhancedInputComponent) return;
    
    // Clear existing bindings if any
    CachedEnhancedInputComponent->RemoveBindingByHandle(ScanStartHandle);
    CachedEnhancedInputComponent->RemoveBindingByHandle(ScanEndHandle);
    
    // Set up main scanning binding based on mode
    if (bUseScanModeToggle)
    {
        ScanStartHandle = CachedEnhancedInputComponent->BindAction(ScanModeAction, ETriggerEvent::Started, this, &ADkPlayerController::ScanModeToggle).GetHandle();
    }
    else
    {
        ScanStartHandle = CachedEnhancedInputComponent->BindAction(ScanModeAction, ETriggerEvent::Started, this, &ADkPlayerController::ScanModeStart).GetHandle();
        ScanEndHandle = CachedEnhancedInputComponent->BindAction(ScanModeAction, ETriggerEvent::Completed, this, &ADkPlayerController::ScanModeEnd).GetHandle();
    }

    // Always bind execute scan
    CachedEnhancedInputComponent->BindAction(ScanExecuteAction, ETriggerEvent::Started, this, &ADkPlayerController::ScanExecuteStart);
    CachedEnhancedInputComponent->BindAction(ScanExecuteAction, ETriggerEvent::Completed, this, &ADkPlayerController::ScanExecuteEnd);
}

void ADkPlayerController::SetTargetingMode(bool bNewToggleMode)
{
    if (bUseTargetModeToggle == bNewToggleMode) return;
    
    bUseTargetModeToggle = bNewToggleMode;
    
    // If switching to hold mode while targeting is active, end targeting
    if (!bUseTargetModeToggle && bIsTargeting)
    {
        TargetEnd();
        bIsTargeting = false;
    }
    
    // Rebind the inputs for the new mode
    SetupTargetingBindings();
}

void ADkPlayerController::ResetTargetingState()
{
    bIsTargeting = false;
    
    // If using toggle mode, ensure we're in a clean state
    if (bUseTargetModeToggle)
    {
        // Re-setup bindings to ensure clean state
        SetupTargetingBindings();
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

void ADkPlayerController::Move(const FInputActionValue& Value)
{
    if (!PlayerRef)
    {
        UE_LOG(LogDkPlayerController, Warning, TEXT("No player ref when moving"));
        return;
    }
    FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator SpringArmRotation = PlayerSpringArmRef->GetTargetRotation();
    // We only want the yaw component for horizontal movement
    const FRotator YawRotation(0, SpringArmRotation.Yaw, 0);
    
    // Get the forward and right vectors based on the spring arm's rotation
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    
    // Apply movement in the rotated directions
    PlayerRef->AddMovementInput(ForwardDirection, MovementVector.Y);
    PlayerRef->AddMovementInput(RightDirection, MovementVector.X);
}

void ADkPlayerController::Look(const FInputActionValue& Value)
{
    if (!PlayerRef) {return;}
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    //UE_LOG(LogDkPlayerController, Warning, TEXT("LookAxisVector: %s"), *LookAxisVector.ToString());
    
    PlayerRef->AddControllerYawInput(LookAxisVector.X * TargetingYawInputScale);
    PlayerRef->AddControllerPitchInput(LookAxisVector.Y);
     
}

void ADkPlayerController::TargetStart()
{
    if (!bIsTargeting || !bUseTargetModeToggle)  // Allow start if not targeting or in hold mode
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
    if (bIsTargeting || !bUseTargetModeToggle)  // Allow end if targeting or in hold mode
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

void ADkPlayerController::Jump()
{
    if (JumpDelegate.IsBound())
    {
        JumpDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Jump Button Pressed"));
}

void ADkPlayerController::Dodge()
{
    if (DodgeDelegate.IsBound())
    {
        DodgeDelegate.Broadcast();
    }
   // UE_LOG(LogTemp, Warning, TEXT("Dodge Button Pressed"));
}

void ADkPlayerController::Attack()
{
    if (AttackDelegate.IsBound())
    {
        AttackDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Attack Button Pressed"));
}

void ADkPlayerController::Interact()
{
    if (InteractDelegate.IsBound())
    {
        InteractDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Interact Button Pressed"));
}

void ADkPlayerController::Drop()
{
    if (DropDelegate.IsBound())
    {
        DropDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Drop Button Pressed"));
}

void ADkPlayerController::Lift()
{
    if (LiftDelegate.IsBound())
    {
        LiftDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Lift Button Pressed"));
}

void ADkPlayerController::TogglePauseMenu()
{
    if (TogglePauseMenuDelegate.IsBound())
    {
        TogglePauseMenuDelegate.Broadcast();
    }
    //UE_LOG(LogTemp, Warning, TEXT("Pause Menu Button Pressed"));
}

void ADkPlayerController::ToggleUpgradeMenu()
{
    if (ToggleUpgradeMenuDelegate.IsBound())
    {
        ToggleUpgradeMenuDelegate.Broadcast();
    }
    UE_LOG(LogTemp, Warning, TEXT("Upgrade Menu Button Pressed"));
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

FScanModeStartSignature* ADkPlayerController::GetScanModeStartDelegate()
{
    return &ScanModeStartDelegate;
}

FScanModeEndSignature* ADkPlayerController::GetScanModeEndDelegate()
{
    return &ScanModeEndDelegate;
}

FScanExecuteStartSignature* ADkPlayerController::GetScanExecuteStartDelegate()
{
    return &ScanExecuteStartDelegate;
}

FScanExecuteEndSignature* ADkPlayerController::GetScanExecuteEndDelegate()
{
    return &ScanExecuteEndDelegate;
}

void ADkPlayerController::ScanModeStart()
{
    if (!bIsScanning || !bUseScanModeToggle)
    {
        if (ScanModeStartDelegate.IsBound())
        {
            ScanModeStartDelegate.Broadcast();
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(ScanMappingContext, 0);
            }
        }
        if (!bUseScanModeToggle)
        {
            bIsScanning = true;
        }
    }
}

void ADkPlayerController::ScanModeEnd()
{
    if (bIsScanning || !bUseScanModeToggle)
    {
        // Execute end scan if it was active
        ScanExecuteEnd();
        
        if (ScanModeEndDelegate.IsBound())
        {
            ScanModeEndDelegate.Broadcast();
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
            {
                Subsystem->RemoveMappingContext(ScanMappingContext);
            }
        }
        if (!bUseScanModeToggle)
        {
            bIsScanning = false;
        }
    }
}

void ADkPlayerController::ScanModeToggle()
{
    if (!bIsScanning)
    {
        ScanModeStart();
    }
    else
    {
        ScanExecuteEnd();  // Add this to ensure scan stops when toggling off
        ScanModeEnd();
    }
    bIsScanning = !bIsScanning;
}

void ADkPlayerController::ScanExecuteStart()
{
    if (!bIsScanning) return;
    
    if (ScanExecuteStartDelegate.IsBound())
    {
        ScanExecuteStartDelegate.Broadcast();
    }
}

void ADkPlayerController::ScanExecuteEnd()
{
    if (!bIsScanning) return;
    
    if (ScanExecuteEndDelegate.IsBound())
    {
        ScanExecuteEndDelegate.Broadcast();
    }
}

void ADkPlayerController::SetScanModeToggle(bool bNewToggleMode)
{
    if (bUseScanModeToggle == bNewToggleMode) return;
    
    bUseScanModeToggle = bNewToggleMode;
    
    if (!bUseScanModeToggle && bIsScanning)
    {
        ScanModeEnd();
        bIsScanning = false;
    }
}

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


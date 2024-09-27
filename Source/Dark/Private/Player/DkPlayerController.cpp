// Copyright @ Christian Reichel

#include "Player/DkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/DkCharacter.h"

DEFINE_LOG_CATEGORY(LogDkPlayerController);

void ADkPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(GetPawn());
		UE_LOG(LogDkPlayerController, Warning, TEXT("PlayerRef set"));
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
	UE_LOG(LogDkPlayerController, Warning, TEXT("MovementVector: %s"), *MovementVector.ToString());
	
		//const FRotator CurrentControlRotation = GetControlRotation();
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);
	    
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	    
		PlayerRef->AddMovementInput(ForwardDirection, MovementVector.Y);
		PlayerRef->AddMovementInput(RightDirection, MovementVector.X);
}

void ADkPlayerController::Look(const FInputActionValue& Value)
{
	if (!PlayerRef) {return;}
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	UE_LOG(LogDkPlayerController, Warning, TEXT("MovementVector: %s"), *LookAxisVector.ToString());
	
		PlayerRef->AddControllerYawInput(LookAxisVector.X);
		PlayerRef->AddControllerPitchInput(LookAxisVector.Y);
}

void ADkPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADkPlayerController::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADkPlayerController::Look);
	}
	else
	{
		UE_LOG(LogDkPlayerController, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}


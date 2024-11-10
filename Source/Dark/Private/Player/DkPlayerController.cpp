// Copyright @ Christian Reichel

#include "Player/DkPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"

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
	if (PlayerRef)
	{
		PlayerSpringArmRef = PlayerRef->FindComponentByClass<USpringArmComponent>();
	}

	

	//Targeting UI
	if (LetterboxWidgetClass)
	{
		LetterboxWidget = CreateWidget<UUserWidget>(this, LetterboxWidgetClass);
	}
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

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADkPlayerController::Jump);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADkPlayerController::Attack);

		//Targeting
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ADkPlayerController::TargetStart);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Completed, this, &ADkPlayerController::TargetEnd);
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
	
	PlayerRef->AddControllerYawInput(LookAxisVector.X);
	PlayerRef->AddControllerPitchInput(LookAxisVector.Y);
}

void ADkPlayerController::TargetStart()
{
	if (TargetStartDelegate.IsBound())
	{
		TargetStartDelegate.Broadcast();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Target Button Pressed"));
}

void ADkPlayerController::TargetEnd()
{
	if (TargetEndDelegate.IsBound())
	{
		TargetEndDelegate.Broadcast();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Target Button Released"));
}

void ADkPlayerController::Jump()
{
	if (JumpDelegate.IsBound())
	{
		JumpDelegate.Broadcast();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Jump Button Pressed"));
}

void ADkPlayerController::Attack()
{
	if (AttackDelegate.IsBound())
	{
		AttackDelegate.Broadcast();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Attack Button Pressed"));
}

FTargetStartSignature* ADkPlayerController::GetTargetStartDelegate()
{
	return &TargetStartDelegate;
}

FTargetEndSignature* ADkPlayerController::GetTargetEndDelegate()
{
	return &TargetEndDelegate;
}

FJumpSignature* ADkPlayerController::GetJumpDelegate()
{
	return &JumpDelegate;
}

FAttackSignature* ADkPlayerController::GetAttackDelegate()
{
	return &AttackDelegate;
}


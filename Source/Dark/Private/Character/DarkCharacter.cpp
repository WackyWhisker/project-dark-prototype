// Copyright @ Christian Reichel


#include "Character/DarkCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(LogDarkCharacter);

ADarkCharacter::ADarkCharacter()
{
	//Don't rotate the character in any way through mouse/stick input
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//Ensure the character rotates towards movement input, at a certain speed (TODO: not when aiming at enemy)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	//Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true; //rotate through mouse/stick input

	//Third person follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //don't rotate through mouse/stick input
	
}

void ADarkCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADarkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADarkCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADarkCharacter::Look);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADarkCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADarkCharacter::StopJumping);
	}
	else
	{
		UE_LOG(LogDarkCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADarkCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	UE_LOG(LogDarkCharacter, Warning, TEXT("MovementVector: %s"), *MovementVector.ToString());
	
	if (Controller != nullptr)
	{
	    const FRotator ControlRotation = Controller->GetControlRotation();
	    const FRotator YawRotation(0, ControlRotation.Yaw, 0);
	    
	    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	    
	    AddMovementInput(ForwardDirection, MovementVector.Y);
	    AddMovementInput(RightDirection, MovementVector.X);
	}

}

void ADarkCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	UE_LOG(LogDarkCharacter, Warning, TEXT("MovementVector: %s"), *LookAxisVector.ToString());
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADarkCharacter::Jump()
{
	Super::Jump();
}

void ADarkCharacter::StopJumping()
{
	Super::StopJumping();
}





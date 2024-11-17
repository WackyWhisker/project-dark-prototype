// Copyright @ Christian Reichel

#include "Character/DkCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogDkCharacter);

ADkCharacter::ADkCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Don't rotate the character in any way through mouse/stick input
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//Ensure the character rotates towards movement input, at a certain speed
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	//Weapon mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	
	
	//Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true; //rotate through mouse/stick input

	//Third person follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //don't rotate through mouse/stick input

	//State Machine defaults
	StateManager = CreateDefaultSubobject<UDkStateManagerComponent>("StateManager");

	//Player States
	DkPlayerState = EDkPlayerAnimationState::Default;
}

void ADkCharacter::AttachWeaponToSocket(FName SocketName)
{
	if (!WeaponMesh) return;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(GetMesh(), AttachRules, SocketName);
	WeaponMesh->ComponentTags.Add(FName("WeaponTag"));
}

void ADkCharacter::BeginPlay()
{
	Super::BeginPlay();
	StateManager->InitializeStateManager();
	AttachWeaponToSocket("HolsterWeaponSocket");
}




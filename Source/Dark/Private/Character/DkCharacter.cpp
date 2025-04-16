// Copyright @ Christian Reichel

#include "Character/DkCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Component/DkDamageFlashComponent.h"
#include "Core/DkGameModeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystem/DkGameStateSubsystem.h"
#include "AbilitySystem/AbilitySystemComponent/DkAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/DkGenericAttributeSet.h"
#include "AbilitySystem/Data/DkGameplayAbilityData.h"

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

	//Adding flash component
	FlashComponent = CreateDefaultSubobject<UDkDamageFlashComponent>(TEXT("FlashComponent"));
	
	
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
	DkPlayerAimingAnimationState = EDkPlayerAimingAnimationState::None;

	//Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UDkAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	GenericAttributes = CreateDefaultSubobject<UDkGenericAttributeSet>(TEXT("GenericAttributes"));
	
}

void ADkCharacter::AttachWeaponToSocket(FName SocketName)
{
	if (!WeaponMesh) return;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(GetMesh(), AttachRules, SocketName);
	WeaponMesh->ComponentTags.Add(FName("WeaponTag"));
}

void ADkCharacter::HandleDeath_Implementation()
{
	if (UDkGameStateSubsystem* GameStateSubsystem = 
			GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
	{
		GameStateSubsystem->BeginDeathSequence();
	}
}

UAbilitySystemComponent* ADkCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (GameplayAbilityData)
		{
			const TSet<FGameplayInputAbilityInfo>& InputAbilities = GameplayAbilityData->GetInputAbilities();
			for (const auto& It : InputAbilities)
			{
				if (It.IsValid())
				{
					const UInputAction* InputAction = It.InputAction;
					const int32 InputID = It.InputID;
     
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &ADkCharacter::OnAbilityInputPressed, InputID);
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &ADkCharacter::OnAbilityInputReleased, InputID);
				}
			}
		}
	}
}

void ADkCharacter::InitAbilitySystem()
{
	if (GameplayAbilityData)
	{
		const TSet<FGameplayInputAbilityInfo>& InputAbilities = GameplayAbilityData->GetInputAbilities();
		constexpr int32 AbilityLevel = 1;
  
		for (const auto& It : InputAbilities)
		{
			if (It.IsValid() && It.bGiveByDefault) //give only if marked as give by default. that way it receives an input id in the data asset for later, but not every single set up ability is given directly at start.
			{
				const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(It.GameplayAbilityClass, AbilityLevel, It.InputID);
				AbilitySystemComponent->GiveAbility(AbilitySpec);
			}
		}
	}
}

void ADkCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void ADkCharacter::OnAbilityInputReleased(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}

void ADkCharacter::BeginPlay()
{
	Super::BeginPlay();
	//StateManager->InitializeStateManager();
	AttachWeaponToSocket("HolsterWeaponSocket");
	if (FlashComponent)
	{
		FlashComponent->SetupMesh(GetMesh());
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitAbilitySystem();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			constexpr int32 Priority = 0;
			Subsystem->AddMappingContext(DefaultAbilityMappingContext, Priority);
		}
	}
}

void ADkCharacter::Destroyed()
{
	Super::Destroyed();//TODO: Remove after playtest
	if (ADkGameModeBase* DkGameModeBase = Cast<ADkGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		DkGameModeBase->RestartLevelDelayed();
	}
}




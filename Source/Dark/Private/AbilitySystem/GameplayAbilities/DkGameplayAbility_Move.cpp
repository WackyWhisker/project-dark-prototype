// Copyright @ Christian Reichel

#include "AbilitySystem/GameplayAbilities/DkGameplayAbility_Move.h"
#include "Character/DkCharacter.h"
#include "GameFramework/SpringArmComponent.h"

void UDkGameplayAbility_Move::OnTriggeredInputAction(const FInputActionValue& Value)
{
	Super::OnTriggeredInputAction(Value);
	//UE_LOG(LogTemp, Warning, TEXT("Move Ability Triggered"));
    
	ADkCharacter* PlayerRef = Cast<ADkCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerRef)
		return;
    
	USpringArmComponent* PlayerSpringArmRef = PlayerRef->FindComponentByClass<USpringArmComponent>();
	if (!PlayerSpringArmRef)
		return;
        
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0, PlayerSpringArmRef->GetTargetRotation().Yaw, 0);
    
	// Get directional vectors based on camera orientation
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    
	// Apply movement
	PlayerRef->AddMovementInput(ForwardDirection, MovementVector.Y);
	PlayerRef->AddMovementInput(RightDirection, MovementVector.X);
}

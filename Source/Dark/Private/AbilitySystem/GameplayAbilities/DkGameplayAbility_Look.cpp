// Copyright @ Christian Reichel


#include "AbilitySystem/GameplayAbilities/DkGameplayAbility_Look.h"

#include "Character/DkCharacter.h"

void UDkGameplayAbility_Look::OnTriggeredInputAction(const FInputActionValue& Value)
{
	Super::OnTriggeredInputAction(Value);
	//UE_LOG(LogTemp, Warning, TEXT("Look Ability Triggered"));

	ADkCharacter* PlayerRef = Cast<ADkCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerRef)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
    PlayerRef->AddControllerYawInput(LookAxisVector.X);
	PlayerRef->AddControllerPitchInput(LookAxisVector.Y);
}

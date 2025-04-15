// Copyright @ Christian Reichel


#include "AbilitySystem/GameplayAbilities/DkGameplayAbility.h"

UDkGameplayAbility::UDkGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

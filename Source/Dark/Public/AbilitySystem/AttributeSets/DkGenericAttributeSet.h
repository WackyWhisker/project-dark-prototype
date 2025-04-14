// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/AbilitySystemComponent/DkAbilitySystemComponent.h"
#include "DkGenericAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DARK_API UDkGenericAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UDkGenericAttributeSet();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;

	ATTRIBUTE_ACCESSORS(UDkGenericAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UDkGenericAttributeSet, MaxHealth);
	
};

//TODO: Check if I need to implement 'GetLifetimeReplicatedProps' in Singleplayer games.
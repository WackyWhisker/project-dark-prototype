// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DkGameplayAbility.generated.h"


UCLASS()
class DARK_API UDkGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UDkGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

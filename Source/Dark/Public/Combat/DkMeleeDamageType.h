// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DkMeleeDamageType.generated.h"

UCLASS()
class DARK_API UDkMeleeDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UDkMeleeDamageType()
	{
		bCausedByWorld = false;
	}

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.0f;
};

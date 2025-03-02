// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DkBindingDamageType.generated.h"

UCLASS()
class DARK_API UDkBindingDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UDkBindingDamageType()
	{
		bCausedByWorld = false;
	}
};

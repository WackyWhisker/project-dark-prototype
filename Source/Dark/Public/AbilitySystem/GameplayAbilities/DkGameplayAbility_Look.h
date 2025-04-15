// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkGameplayAbility_TriggeredInputActionAbility.h"
#include "DkGameplayAbility_Look.generated.h"


UCLASS()
class DARK_API UDkGameplayAbility_Look : public UDkGameplayAbility_TriggeredInputActionAbility
{
	GENERATED_BODY()
	
protected:
	//~ Begin UGameplayAbility_BaseTriggeredInputActionAbility Interface
	virtual void OnTriggeredInputAction(const FInputActionValue& Value);
	//~ End UGameplayAbility_BaseTriggeredInputActionAbility Interface
};

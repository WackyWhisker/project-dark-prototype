// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/States/DkStateBase.h"
#include "DkPlayerStateBase.generated.h"

class ADkCharacter;

UCLASS()
class DARK_API UDkPlayerStateBase : public UDkStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ADkCharacter* PlayerRef = nullptr;

	virtual void OnStateEnter(AActor* StateOwner) override;
	virtual void OnStateExit() override;
	
	
};

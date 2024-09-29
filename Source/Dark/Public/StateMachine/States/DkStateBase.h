// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DkStateBase.generated.h"

UCLASS(Blueprintable)
class DARK_API UDkStateBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bCanStateTick = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bCanStateRepeat = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	FName StateDisplayName;

	virtual void OnStateEnter(AActor* StateOwner);
	virtual void TickState();
	virtual void OnStateExit();
};

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	UPROPERTY()
	TObjectPtr<UDkStateBase> PreviousState;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	TArray<FString> RestrictedFromStates;

	UFUNCTION(BlueprintCallable, Category = "State")
	UDkStateBase* GetPreviousState() const {return PreviousState; }
	
};

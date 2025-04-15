// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkGameplayAbility.h"
#include "EnhancedInputComponent.h"
#include "DkGameplayAbility_TriggeredInputActionAbility.generated.h"

struct FInputActionValue;

UCLASS()
class DARK_API UDkGameplayAbility_TriggeredInputActionAbility : public UDkGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDkGameplayAbility_TriggeredInputActionAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
protected:
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;

	TArray<uint32> TriggeredEventHandles;

	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~ End UGameplayAbility Interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bCancelAbilityOnInputReleased;

protected:
	//~ Begin UGameplayAbility_BaseTriggeredInputActionAbility Interface
	virtual void OnTriggeredInputAction(const FInputActionValue& Value);
	//~ End UGameplayAbility_BaseTriggeredInputActionAbility Interface
};
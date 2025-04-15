// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/GameplayAbilities/DkGameplayAbility.h"
#include "DkGameplayAbilityData.generated.h"

class UInputAction;

USTRUCT()
struct FGameplayInputAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GameplayAbilityInputInfo")
	TSubclassOf<UDkGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditAnywhere, Category = "GameplayAbilityInputInfo")
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, Category = "GameplayAbilityInputInfo")
	bool bGiveByDefault = true;

	//InputID will be generated automatically
	UPROPERTY(VisibleAnywhere, Category = "GameplayAbilityInputInfo")
	int32 InputID;

	bool IsValid() const
	{
		return GameplayAbilityClass && InputAction;
	}

	bool operator==(const FGameplayInputAbilityInfo& Other) const
	{
		return GameplayAbilityClass == Other.GameplayAbilityClass && InputAction == Other.InputAction;
	}

	bool operator!=(const FGameplayInputAbilityInfo& Other) const
	{
		//return GameplayAbilityClass != Other.GameplayAbilityClass && InputAction!=Other.InputAction; // long version
		return !operator==(Other); //Short version, just return the opposite of the other one
	}

	friend uint32 GetTypeHash(const FGameplayInputAbilityInfo& Item)
	{
		return HashCombine(GetTypeHash(Item.GameplayAbilityClass), GetTypeHash(Item.InputAction));
	}

	FGameplayInputAbilityInfo()  : InputID(INDEX_NONE)	{};
	
};


UCLASS()
class DARK_API UDkGameplayAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	const TSet<FGameplayInputAbilityInfo>& GetInputAbilities() const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSet<FGameplayInputAbilityInfo> InputAbilities;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Character/DkCharacterBase.h"
#include "Interface/DkDeathHandlerInterface.h"
#include "Logging/LogMacros.h"
#include "StateMachine/DkStateManagerComponent.h"
#include "DkCharacter.generated.h"

class UAbilitySystemComponent;
class UDkGameplayAbilityData;
class UDkAbilitySystemComponent;
class UDkGenericAttributeSet;
class USpringArmComponent;
class UCameraComponent;
class UDkDamageFlashComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum EDkPlayerAnimationState
{
	Idle,
	Air,
	Run,
	Jump,
	Fall,
	Land,
	Attack,
	Hang,
	ClimbUp,
	Default
};

UENUM(BlueprintType)
enum EDkPlayerAimingAnimationState
{
	Firearm,
	Scanning,
	None
};

DECLARE_LOG_CATEGORY_EXTERN(LogDkCharacter, Log, All);

UCLASS()
class DARK_API ADkCharacter : public ADkCharacterBase, public IDkDeathHandlerInterface
{
	GENERATED_BODY()

public:
	explicit ADkCharacter(const FObjectInitializer& ObjectInitializer);
	//TODO: Consider getter methods for camera and spring arm (see template)

	//State Machine stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StateManager)
	UDkStateManagerComponent* StateManager;

	UPROPERTY(BlueprintReadOnly, Category = StateManager)
	TEnumAsByte<EDkPlayerAnimationState> DkPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = StateManager)
	TEnumAsByte<EDkPlayerAimingAnimationState> DkPlayerAimingAnimationState;

	UFUNCTION(BlueprintCallable, Category = Combat)
	void AttachWeaponToSocket(FName SocketName);

	//Death Handler Interface
	virtual void HandleDeath_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	UDkDamageFlashComponent* FlashComponent;

	//Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	//Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	TObjectPtr<UDkAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDkGenericAttributeSet> GenericAttributes;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UDkGameplayAbilityData* GetPlayerGameplayAbilitiesDataAsset() const { return GameplayAbilityData; }
	

private:
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//Weapons TODO: see if a weapon manager would be needed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	//Ability System Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultAbilityMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDkGameplayAbilityData> GameplayAbilityData;

	void InitAbilitySystem();
	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);
	
};

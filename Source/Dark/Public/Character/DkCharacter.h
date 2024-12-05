// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Character/DkCharacterBase.h"
#include "Logging/LogMacros.h"
#include "StateMachine/DkStateManagerComponent.h"
#include "DkCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
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

DECLARE_LOG_CATEGORY_EXTERN(LogDkCharacter, Log, All);

UCLASS()
class DARK_API ADkCharacter : public ADkCharacterBase
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

	UFUNCTION(BlueprintCallable, Category = Combat)
	void AttachWeaponToSocket(FName SocketName);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	
	
private:
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//Weapons TODO: see if a weapon manager would be needed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;
	
};

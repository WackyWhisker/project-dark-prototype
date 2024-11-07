// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkPlayerControllerInterface.h"
#include "GameFramework/PlayerController.h"
#include "DkPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ADkCharacter;
class UUserWidget;
class USpringArmComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogDkPlayerController, Log, All);
/**
 * 
 */
UCLASS()
class DARK_API ADkPlayerController : public APlayerController, public IDkPlayerControllerInterface
{
	GENERATED_BODY()

public:
	//Input methods corresponding to the input action
	void Jump();
	void Attack();
	void TargetStart();
	void TargetEnd();

	//Targeting UI
	UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
	TSubclassOf<UUserWidget> LetterboxWidgetClass;

	UFUNCTION(BlueprintCallable, Category = UI_Targeting)
	void ToggleLetterboxUI(bool bShowLetterboxUI);

	UPROPERTY()
	USpringArmComponent* PlayerSpringArmRef;
	
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//Player Controller Interface Overrides
	virtual FJumpSignature* GetJumpDelegate() override;
	virtual FAttackSignature* GetAttackDelegate() override;
	virtual FTargetStartSignature* GetTargetStartDelegate() override;
	virtual FTargetEndSignature* GetTargetEndDelegate() override;


private:
	//Controlled player
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	ADkCharacter* PlayerRef = nullptr;
	
	//Input actions and mappings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetAction;

	//Player Controller Interface Delegates
	FJumpSignature JumpDelegate;
	FAttackSignature AttackDelegate;
	FTargetStartSignature TargetStartDelegate;
	FTargetEndSignature TargetEndDelegate;

	//Targeting UI
	UPROPERTY()
	UUserWidget* LetterboxWidget;

	UPROPERTY()
	float FadeDuration;
};

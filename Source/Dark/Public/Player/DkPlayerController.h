// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DkPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ADkCharacter;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogDkPlayerController, Log, All);
/**
 * 
 */
UCLASS()
class DARK_API ADkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

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
};

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

UCLASS()
class DARK_API ADkPlayerController : public APlayerController, public IDkPlayerControllerInterface
{
    GENERATED_BODY()

public:
    // public methods
    void Jump();
    void Dodge();
    void Attack();
    void TargetStart();
    void TargetEnd();
    void TargetToggle();
    void TargetCycleLeft();
    void TargetCycleRight();
    void Drop();
    void Lift();
    void SetMappingContext(const FName& ContextName, bool bEnable);
    void ToggleLetterboxUI(bool bShowLetterboxUI);

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void SetTargetingMode(bool bNewToggleMode);

    UFUNCTION(BlueprintPure, Category = "Targeting")
    bool IsUsingToggleMode() const { return bUseToggleMode; }

public:
    // public properties
    UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
    TSubclassOf<UUserWidget> LetterboxWidgetClass;

    UPROPERTY()
    USpringArmComponent* PlayerSpringArmRef;

    UPROPERTY()
    float TargetingYawInputScale = 1.0f;

protected:
    // protected methods
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    void SetupTargetingBindings();
    virtual FJumpSignature* GetJumpDelegate() override;
    virtual FDodgeSignature* GetDodgeDelegate() override;
    virtual FAttackSignature* GetAttackDelegate() override;
    virtual FTargetStartSignature* GetTargetStartDelegate() override;
    virtual FTargetEndSignature* GetTargetEndDelegate() override;
    virtual FTargetCycleLeftSignature* GetTargetCycleLeftDelegate() override;
    virtual FTargetCycleRightSignature* GetTargetCycleRightDelegate() override;
    virtual FDropSignature* GetDropDelegate() override;
    virtual FLiftSignature* GetLiftDelegate() override;

private:
    // private properties
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
    ADkCharacter* PlayerRef = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* TargetMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* LedgeHangMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    bool bUseToggleMode = true;

    UPROPERTY()
    bool bIsTargeting = false;

    UPROPERTY()
    UEnhancedInputComponent* CachedEnhancedInputComponent;

    uint32 TargetStartHandle;
    uint32 TargetEndHandle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DodgeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AttackAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* TargetAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* TargetCycleLeftAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* TargetCycleRightAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DropAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LiftAction;

    FJumpSignature JumpDelegate;
    FDodgeSignature DodgeDelegate;
    FAttackSignature AttackDelegate;
    FTargetStartSignature TargetStartDelegate;
    FTargetEndSignature TargetEndDelegate;
    FTargetCycleLeftSignature TargetCycleLeftDelegate;
    FTargetCycleRightSignature TargetCycleRightDelegate;
    FDropSignature DropDelegate;
    FLiftSignature LiftDelegate;

    UPROPERTY()
    UUserWidget* LetterboxWidget;

    UPROPERTY()
    float FadeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TMap<FName, UInputMappingContext*> MappingContexts;
};
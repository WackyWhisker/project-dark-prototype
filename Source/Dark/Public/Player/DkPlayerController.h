#pragma once

#include "CoreMinimal.h"
#include "DkPlayerControllerInterface.h"
#include "LevelSequencePlayer.h"
#include "LevelSequence.h"
#include "GameFramework/PlayerController.h"
#include "Subsystem/DkGameStateSubsystem.h"
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
   void Interact();
   void TargetStart();
   void TargetEnd();
   void TargetToggle();
   void TargetCycleLeft();
   void TargetCycleRight();
   void Drop();
   void Lift();
   void TogglePauseMenu();
   void SetMappingContext(const FName& ContextName, bool bEnable);
   void ToggleLetterboxUI(bool bShowLetterboxUI);
   void ScanModeStart();
   void ScanModeEnd();
   void ScanModeToggle();
   void ScanExecuteStart();
   void ScanExecuteEnd();

   UFUNCTION(Exec)
   void ToggleTargetMode();

   UFUNCTION(Exec)
   void ToggleScanMode();

   UFUNCTION(BlueprintCallable, Category = "Scanning")
   void SetScanModeToggle(bool bNewToggleMode);

   UFUNCTION(BlueprintCallable, Category = "Targeting")
   void SetTargetingMode(bool bNewToggleMode);

   UFUNCTION(BlueprintPure, Category = "Targeting")
   bool IsUsingToggleMode() const { return bUseTargetModeToggle; }

public:
   UFUNCTION(BlueprintCallable, Category = "Targeting")
   void ResetTargetingState();

public:
   // public properties
   UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
   TSubclassOf<UUserWidget> LetterboxWidgetClass;

   UPROPERTY()
   USpringArmComponent* PlayerSpringArmRef;

   UPROPERTY()
   float TargetingYawInputScale = 1.0f;

   virtual FJumpSignature* GetJumpDelegate() override;
   virtual FDodgeSignature* GetDodgeDelegate() override;
   virtual FAttackSignature* GetAttackDelegate() override;
   virtual FTargetStartSignature* GetTargetStartDelegate() override;
   virtual FTargetEndSignature* GetTargetEndDelegate() override;
   virtual FTargetCycleLeftSignature* GetTargetCycleLeftDelegate() override;
   virtual FTargetCycleRightSignature* GetTargetCycleRightDelegate() override;
   virtual FDropSignature* GetDropDelegate() override;
   virtual FLiftSignature* GetLiftDelegate() override;
   virtual FTogglePauseMenuSignature* GetTogglePauseMenuDelegate() override;
   virtual FInteractSignature* GetInteractDelegate() override;
   virtual FScanModeStartSignature* GetScanModeStartDelegate() override;
   virtual FScanModeEndSignature* GetScanModeEndDelegate() override;
   virtual FScanExecuteStartSignature* GetScanExecuteStartDelegate() override;
   virtual FScanExecuteEndSignature* GetScanExecuteEndDelegate() override;

protected:
   // protected methods
   void Move(const FInputActionValue& Value);
   void Look(const FInputActionValue& Value);
   virtual void BeginPlay() override;
   virtual void SetupInputComponent() override;
   void SetupTargetingBindings();
   void SetupScanBindings();
   

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
   UInputMappingContext* ScanMappingContext;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   bool bUseTargetModeToggle = true;

   UPROPERTY()
   bool bIsTargeting = false;

   UPROPERTY()
   UEnhancedInputComponent* CachedEnhancedInputComponent;

   uint32 TargetStartHandle;
   uint32 TargetEndHandle;

   uint32 ScanStartHandle;
   uint32 ScanEndHandle;

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

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* TogglePauseMenuAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* InteractAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* ScanModeAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true")) 
   UInputAction* ScanExecuteAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   bool bUseScanModeToggle = true;

   UPROPERTY()
   bool bIsScanning = false;

   FJumpSignature JumpDelegate;
   FDodgeSignature DodgeDelegate;
   FAttackSignature AttackDelegate;
   FTargetStartSignature TargetStartDelegate;
   FTargetEndSignature TargetEndDelegate;
   FTargetCycleLeftSignature TargetCycleLeftDelegate;
   FTargetCycleRightSignature TargetCycleRightDelegate;
   FDropSignature DropDelegate;
   FLiftSignature LiftDelegate;
   FTogglePauseMenuSignature TogglePauseMenuDelegate;
   FInteractSignature InteractDelegate;
   FScanModeStartSignature ScanModeStartDelegate;
   FScanModeEndSignature ScanModeEndDelegate;
   FScanExecuteStartSignature ScanExecuteStartDelegate;
   FScanExecuteEndSignature ScanExecuteEndDelegate;

   UPROPERTY()
   UUserWidget* LetterboxWidget;

   UPROPERTY()
   float FadeDuration;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   TMap<FName, UInputMappingContext*> MappingContexts;

   //Castle Game State specifics
   UFUNCTION()
   void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);

   // Optional: Keep track if we disabled input due to death
   bool bWasInputDisabledByDeath = false;

private:
   UPROPERTY(EditDefaultsOnly, Category = "Sequences")
   ULevelSequence* DeathSequence;

   UPROPERTY(EditDefaultsOnly, Category = "Sequences")
   ULevelSequence* RespawnSequence;

   // Keep track of sequence player
   UPROPERTY()
   ULevelSequencePlayer* ActiveSequencePlayer;

   void PlayDeathSequence();
   void PlayRespawnSequence();
   
   UFUNCTION()
   void OnDeathSequenceFinished();

   UFUNCTION()
   void OnRespawnSequenceFinished();
};
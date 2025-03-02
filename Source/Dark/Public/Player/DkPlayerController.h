// Copyright @ Christian Reichel

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
   // Basic player actions
   void Jump();
   void Dodge();
   void Attack();
   void Interact();
   void Drop();
   void Lift();
   
   // Menu controls
   void TogglePauseMenu();
   void ToggleUpgradeMenu();
   
   // Targeting system
   void TargetStart();
   void TargetEnd();
   void TargetToggle();
   void TargetCycleLeft();
   void TargetCycleRight();
   
   UFUNCTION(BlueprintCallable, Category = "Targeting")
   void SetTargetingMode(bool bNewToggleMode);
   
   UFUNCTION(BlueprintPure, Category = "Targeting")
   bool IsUsingToggleMode() const { return bUseTargetModeToggle; }
   
   UFUNCTION(BlueprintCallable, Category = "Targeting")
   void ResetTargetingState();
   
   UFUNCTION(Exec)
   void ToggleTargetMode();
   
   // Scanning system
   void ScanModeStart();
   void ScanModeEnd();
   void ScanModeToggle();
   void ScanExecuteStart();
   void ScanExecuteEnd();
   
   UFUNCTION(BlueprintCallable, Category = "Scanning")
   void SetScanModeToggle(bool bNewToggleMode);
   
   UFUNCTION(Exec)
   void ToggleScanMode();
   
   // Aiming system
   void Shoot();
   void AimStart();
   void AimEnd();
   void AimToggle();
   
   UFUNCTION(BlueprintCallable, Category = "Aiming")
   void SetAimingMode(bool bNewToggleMode);
   
   UFUNCTION(Exec)
   void ToggleAimMode();
   
   // UI and mapping context
   void SetMappingContext(const FName& ContextName, bool bEnable);
   void ToggleLetterboxUI(bool bShowLetterboxUI);

public:
   // public properties
   UPROPERTY(EditDefaultsOnly, Category = UI_Targeting)
   TSubclassOf<UUserWidget> LetterboxWidgetClass;

   UPROPERTY()
   USpringArmComponent* PlayerSpringArmRef;

   UPROPERTY()
   float TargetingYawInputScale = 1.0f;

   // Interface method implementations
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
   virtual FToggleUpgradeMenuSignature* GetToggleUpgradeMenuDelegate() override;
   virtual FInteractSignature* GetInteractDelegate() override;
   virtual FScanModeStartSignature* GetScanModeStartDelegate() override;
   virtual FScanModeEndSignature* GetScanModeEndDelegate() override;
   virtual FScanExecuteStartSignature* GetScanExecuteStartDelegate() override;
   virtual FScanExecuteEndSignature* GetScanExecuteEndDelegate() override;
   virtual FShootSignature* GetShootDelegate() override;
   virtual FAimStartSignature* GetAimStartDelegate() override;
   virtual FAimEndSignature* GetAimEndDelegate() override;

protected:
   // Base controller methods
   virtual void BeginPlay() override;
   virtual void SetupInputComponent() override;
   
   // Input handling
   void Move(const FInputActionValue& Value);
   void Look(const FInputActionValue& Value);
   
   // Setup methods
   void SetupTargetingBindings();
   void SetupScanBindings();
   void SetupAimingBindings();

private:
   // Character reference
   UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
   ADkCharacter* PlayerRef = nullptr;

   // Input mapping contexts
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputMappingContext* DefaultMappingContext;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputMappingContext* TargetMappingContext;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputMappingContext* LedgeHangMappingContext;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputMappingContext* ScanMappingContext;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputMappingContext* AimingMappingContext;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   TMap<FName, UInputMappingContext*> MappingContexts;

   // Input actions
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
   UInputAction* InteractAction;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* DropAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* LiftAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* TogglePauseMenuAction;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* ToggleUpgradeMenuAction;
   
   // Targeting related properties
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* TargetAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* TargetCycleLeftAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* TargetCycleRightAction;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   bool bUseTargetModeToggle = true;

   UPROPERTY()
   bool bIsTargeting = false;
   
   uint32 TargetStartHandle;
   uint32 TargetEndHandle;
   
   // Scanning related properties
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* ScanModeAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true")) 
   UInputAction* ScanExecuteAction;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   bool bUseScanModeToggle = true;

   UPROPERTY()
   bool bIsScanning = false;
   
   uint32 ScanStartHandle;
   uint32 ScanEndHandle;
   
   // Aiming related properties
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* ShootAction;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   UInputAction* AimAction;
   
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
   bool bUseAimModeToggle = true;

   UPROPERTY()
   bool bIsAiming = false;
   
   uint32 AimStartHandle;
   uint32 AimEndHandle;

   // Input component cache
   UPROPERTY()
   UEnhancedInputComponent* CachedEnhancedInputComponent;

   // Delegates
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
   FToggleUpgradeMenuSignature ToggleUpgradeMenuDelegate;
   FInteractSignature InteractDelegate;
   FScanModeStartSignature ScanModeStartDelegate;
   FScanModeEndSignature ScanModeEndDelegate;
   FScanExecuteStartSignature ScanExecuteStartDelegate;
   FScanExecuteEndSignature ScanExecuteEndDelegate;
   FShootSignature ShootDelegate;
   FAimStartSignature AimStartDelegate;
   FAimEndSignature AimEndDelegate;

   // UI related properties
   UPROPERTY()
   UUserWidget* LetterboxWidget;

   UPROPERTY()
   float FadeDuration;

   // Game state handling
   UFUNCTION()
   void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);
   
   // Track input state for death/respawn
   bool bWasInputDisabledByDeath = false;

   // Sequence handling
   UPROPERTY(EditDefaultsOnly, Category = "Sequences")
   ULevelSequence* DeathSequence;

   UPROPERTY(EditDefaultsOnly, Category = "Sequences")
   ULevelSequence* RespawnSequence;

   UPROPERTY()
   ULevelSequencePlayer* ActiveSequencePlayer;

   void PlayDeathSequence();
   void PlayRespawnSequence();
   
   UFUNCTION()
   void OnDeathSequenceFinished();

   UFUNCTION()
   void OnRespawnSequenceFinished();
};
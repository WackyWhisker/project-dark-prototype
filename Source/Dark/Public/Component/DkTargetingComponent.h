// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/DkCharacter.h"
#include "Player/DkPlayerController.h"
#include "DkTargetingComponent.generated.h"

class IDkPlayerControllerInterface;
class ADkEnemyBase;
class UCameraComponent;
class USpringArmComponent;
class UDkHealthComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDkTargetingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	

	UPROPERTY(BlueprintReadOnly)
	ADkCharacter* PlayerRef = nullptr;

	UPROPERTY(BlueprintReadOnly)
	ADkPlayerController* PlayerControllerRef = nullptr;

	//Enemy targeting camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float TargetArmLength = 800.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float CameraDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float CameraHeight = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float InterpSpeed = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float MinFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float MaxFOV = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float MinTargetDistance = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
	bool bIsTargeting = false;


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializeComponent() override;
	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;


	//Improved Target Selection
	UFUNCTION()
	bool FindBestTarget(AActor*& OutBestTarget, float SphereRadius, float MaxAngleInDegrees);
	
	UFUNCTION()
	bool GetTargetableActorsInRange(TArray<AActor*>& OutTargetableActors, float SphereRadius);
	
	UFUNCTION()
	bool FilterTargetsByViewCone(const TArray<AActor*>& PotentialTargets, 
										   TArray<AActor*>& OutValidTargets, 
										   float MaxAngleInDegrees);

	UFUNCTION()
	bool CycleTarget(bool bGoingRight);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	float TargetableActorsScanRange = 1500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	float MaxViewConeAngle = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
	float PlayerRotationInterpolationSpeed = 50.0f;


private:
	UFUNCTION()
	void OnTargetStart();
	
	UFUNCTION()
	void OnTargetEnd();

	UFUNCTION()
	void OnTargetCycleLeft();

	UFUNCTION()
	void OnTargetCycleRight();

	UFUNCTION()
	void ToggleLetterboxWidget(bool IsTargeting); //always

	UFUNCTION()
	void TogglePlayerComponentValues(bool IsTargeting);

	UFUNCTION()
	void RestoreCameraPositionOnEnd();

	UFUNCTION()
	void ClearTarget(bool IsTargeting); //Only on target end

	//Targeting Tick
	UFUNCTION()
	void HandleEnemyTrackingWhileTargeting(float DeltaTime);

	UFUNCTION()
	void UpdatePlayerRotationWhileTargeting(float DeltaTime);

	UFUNCTION()
	void UpdateCameraWithActiveTarget(float DeltaTime);

	UFUNCTION()
	void UpdatePlayerWithoutActiveTarget(float DeltaTime);

	

	//Enemy targeting
	UPROPERTY(VisibleAnywhere,Category = "Targets")
	TArray<TWeakObjectPtr<AActor>> PossibleTargets;

	UPROPERTY(VisibleAnywhere,Category = "Targets")
	AActor* CurrentActiveTarget;

	UPROPERTY()
	UCameraComponent* PlayerCameraRef = nullptr;

	UPROPERTY()
	USpringArmComponent* PlayerSpringArmRef = nullptr;

	FRotator CalculateIdealSpringArmRotation() const;
	float CalculateIdealSpringArmLength() const;

	FRotator LastUsedTargetRotation;

	void InitiateSweepForTargets();

	UPROPERTY()
	UDkHealthComponent* HealthComponent = nullptr;

	UFUNCTION()
	void OnCurrentTargetHealthDepleted();

private:
	UFUNCTION()
	void HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState);

	
	
};

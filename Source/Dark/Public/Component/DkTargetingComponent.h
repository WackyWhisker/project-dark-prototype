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

	UPROPERTY(BlueprintReadOnly)
	UCameraComponent* PlayerCameraRef = nullptr;

	UPROPERTY(BlueprintReadOnly)
	USpringArmComponent* PlayerSpringArmRef = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

private:
	UFUNCTION()
	void OnTargetStart();
	
	UFUNCTION()
	void OnTargetEnd();

	UFUNCTION()
	void HandlePlayerLocomotion(bool IsTargeting);

	UFUNCTION()
	void HandleLetterboxWidget(bool IsTargeting);

	UFUNCTION()
	void HandleTargetClearing(bool IsTargeting);

	//Targeting Tick
	UFUNCTION()
	void HandleEnemyTrackingWhileTargeting(float DeltaTime);

	UFUNCTION()
	void UpdatePlayerRotationWhileTargeting(float DeltaTime);

	UFUNCTION()
	void UpdateCameraWhileTargeting(float DeltaTime);

	UFUNCTION()
	bool SweepForPossibleTargets(const FVector& Start,
	const float Range,
	const float ConeAngle,
	const float SphereRadius,
	TArray<FHitResult>& OutHits,
	ECollisionChannel TraceChannel,
	const TArray<AActor*>& ActorsToIgnore,
	bool bDrawDebug);

	UFUNCTION()
	AActor* FindClosestTarget(const TArray<FHitResult>& Hits);

	UPROPERTY()
	bool bIsTargeting = false;

	//Enemy targeting
	UPROPERTY(VisibleAnywhere,Category = "Targets")
	TArray<TWeakObjectPtr<AActor>> PossibleTargets;

	UPROPERTY(VisibleAnywhere,Category = "Targets")
	AActor* CurrentActiveTarget;
	
};

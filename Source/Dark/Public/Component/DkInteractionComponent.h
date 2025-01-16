// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "DkInteractionComponent.generated.h"

// Forward declarations
class ADkCharacter;    // Your character class

UCLASS(ClassGroup=(Dark), meta=(BlueprintSpawnableComponent))
class DARK_API UDkInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDkInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxViewConeAngle = 45.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedActor;

	// Player references
	UPROPERTY()
	ADkCharacter* PlayerRef;

	UPROPERTY()
	class UCameraComponent* PlayerCameraRef;

private:
	void UpdateClosestInteractable();
	bool FilterInteractablesByViewCone(const TArray<AActor*>& PotentialTargets, TArray<AActor*>& OutValidTargets);
};

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/DkCharacter.h"
#include "Player/DkPlayerController.h"
#include "DkTargetingComponent.generated.h"

class IDkPlayerControllerInterface;

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

	UPROPERTY()
	bool bIsTargeting = false;
	
};

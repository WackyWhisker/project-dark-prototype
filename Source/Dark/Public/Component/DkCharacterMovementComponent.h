// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DkCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UDkCharacterMovementComponent();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float MaxNonForwardSpeed = 300.0f; 

	virtual float GetMaxSpeed() const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};

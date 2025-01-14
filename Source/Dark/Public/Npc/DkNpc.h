// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkNpcBase.h"
#include "DkNpc.generated.h"

UCLASS()
class DARK_API ADkNpc : public ADkNpcBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADkNpc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

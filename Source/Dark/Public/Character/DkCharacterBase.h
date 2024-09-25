// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DkCharacterBase.generated.h"

UCLASS()
class DARK_API ADkCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADkCharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

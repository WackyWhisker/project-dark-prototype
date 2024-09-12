// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DarkCharacterBase.generated.h"

UCLASS()
class DARK_API ADarkCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADarkCharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

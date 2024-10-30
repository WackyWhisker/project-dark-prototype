// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DkEnemyBase.generated.h"

UCLASS()
class DARK_API ADkEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADkEnemyBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

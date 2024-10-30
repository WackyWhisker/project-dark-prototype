// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkEnemyBase.h"
#include "DkEnemy.generated.h"

UCLASS()
class DARK_API ADkEnemy : public ADkEnemyBase
{
	GENERATED_BODY()

public:
	ADkEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

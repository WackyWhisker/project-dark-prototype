// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkEnemyBase.h"
#include "Interface/DkDeathHandlerInterface.h"
#include "DkEnemy.generated.h"

UCLASS()
class DARK_API ADkEnemy : public ADkEnemyBase, public IDkDeathHandlerInterface
{
	GENERATED_BODY()

public:
	ADkEnemy();

	//Death Handler Interface
	virtual void HandleDeath_Implementation() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

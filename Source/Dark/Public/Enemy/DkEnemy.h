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

private:
	FVector CurrentTargetPoint;
	FVector StartLocation;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float MovementRadius = 150.0f;

	UPROPERTY(EditInstanceOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float MovementSpeed = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 50.0f;
    
private:    
	void GenerateNewTargetPoint();
	void MoveTowardsTarget(float DeltaTime);
	bool HasReachedTarget() const;

};

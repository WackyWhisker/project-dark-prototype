// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetSystem/DkTargetableInterface.h"
#include "DkEnemyBase.generated.h"

UCLASS()
class DARK_API ADkEnemyBase : public ACharacter, public IDkTargetableInterface
{
	GENERATED_BODY()

public:
	ADkEnemyBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

//IDkTargetableInterface functions
	virtual bool CanBeTargeted_Implementation() const override;
	virtual void OnTargeted_Implementation() override;
	virtual void OnUntargeted_Implementation() override;
	virtual FVector GetTargetLocation_Implementation() const override;
	

};

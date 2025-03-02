// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkFirearmBase.h"
#include "DkPistol.generated.h"

UCLASS()
class DARK_API ADkPistol : public ADkFirearmBase
{
	GENERATED_BODY()

public:
	ADkPistol();
    
	virtual void Fire() override;

protected:
	virtual void BeginPlay() override;
    
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Range = 10000.0f;  // 100 meters in UE units

private:
	void ProcessHit(const FHitResult& CameraHit, const FHitResult& MuzzleHit);

};
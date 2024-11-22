// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DkGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DARK_API ADkGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void RestartLevelDelayed();
    
protected:
	UFUNCTION()
	void LoadNewLevel();
	
};

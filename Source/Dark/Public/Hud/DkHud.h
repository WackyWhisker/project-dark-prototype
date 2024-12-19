// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DkHUD.generated.h"

UCLASS()
class DARK_API ADkHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADkHUD();
    
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDkPauseMenuWidget> PauseMenuClass;

private:
	UPROPERTY()
	class UDkPauseMenuWidget* PauseMenu;

	void HandleTogglePauseMenu();
};
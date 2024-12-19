// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DkHUD.generated.h"

class IDkPlayerControllerInterface;

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
	TSubclassOf<class UDkPauseMenuStack> PauseMenuStackClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDkBaseMenuWidget> PauseMenuWidgetClass;

	// protected properties
	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

private:
	UPROPERTY()
	class UDkPauseMenuStack* PauseMenuStack;

	void HandleTogglePauseMenu();

	bool bIsMenuVisible;
};
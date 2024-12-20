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

	UFUNCTION(BlueprintCallable)//should be called from level blueprint?
	void HandleToggleEndScreenMenu();
    
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDkBaseMenuStack> BaseMenuStackClass;

	// protected properties
	IDkPlayerControllerInterface* PlayerControllerInterface = nullptr;

private:
	UPROPERTY()
	class UDkBaseMenuStack* BaseMenuStack;

	void HandleTogglePauseMenu();

	

	bool bIsPauseMenuVisible;
	bool bIsEndScreenMenuVisible;
};
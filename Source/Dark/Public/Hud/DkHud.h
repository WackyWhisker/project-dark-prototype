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

	UFUNCTION(BlueprintCallable)
	void HandleTogglePauseMenu();

	UFUNCTION(BlueprintCallable)
	void HandleToggleUpgradeMenu();
    
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

	
	
	bool bIsPauseMenuVisible;
	bool bIsEndScreenMenuVisible;
	bool bIsUpgradeMenuVisible;
};
// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "DkBaseMenuStack.generated.h"

class UCommonActivatableWidgetStack;

UCLASS()
class DARK_API UDkBaseMenuStack : public UCommonUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Stack;

public:
	/** Gets the widget stack for external access */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	UCommonActivatableWidgetStack* GetStack() const { return Stack; }

	//Events to call in blueprint to handle the individual in-game menus on the stame stack
	
	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowPauseMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHidePauseMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowEndScreenMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHideEndScreenMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowUpgradeMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHideUpgradeMenuWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowRetreatScreenMenuWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHideRetreatScreenMenuWidget();
};

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "DkPauseMenuStack.generated.h"

class UCommonActivatableWidgetStack;

/**
 * Widget class that manages a stack of pause menu widgets
 */
UCLASS()
class DARK_API UDkPauseMenuStack : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Stack;

public:
	/** Gets the widget stack for external access */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	UCommonActivatableWidgetStack* GetStack() const { return Stack; }

	// For events with no parameters
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShowMenuWidgetSignature);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHideMenuWidgetSignature);

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHideMenuWidget();
};
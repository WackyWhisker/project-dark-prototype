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
	
	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnShowMenuWidget();

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHideMenuWidget();
};

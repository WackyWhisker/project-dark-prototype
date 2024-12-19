// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Hud/DkBaseMenuWidget.h"
#include "DkPauseMenuWidget.generated.h"

UCLASS()
class DARK_API UDkPauseMenuWidget : public UDkBaseMenuWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* ResumeButton;
        
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* RestartButton;
    
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* QuitButton;

private:
	UFUNCTION()
	void HandleResumeClicked();
    	    
	UFUNCTION()
	void HandleRestartClicked();
    
	UFUNCTION()
	void HandleQuitClicked();
};
// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkBaseMenuWidget.h"
#include "CommonButtonBase.h"
#include "Player/DkPlayerController.h"
#include "DkEndScreenMenuWidget.generated.h"


UCLASS()
class DARK_API UDkEndScreenMenuWidget : public UDkBaseMenuWidget
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

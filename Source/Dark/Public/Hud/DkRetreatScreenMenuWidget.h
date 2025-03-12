// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "DkBaseMenuWidget.h"
#include "CommonButtonBase.h"
#include "Player/DkPlayerController.h"
#include "DkRetreatScreenMenuWidget.generated.h"

UCLASS()
class DARK_API UDkRetreatScreenMenuWidget : public UDkBaseMenuWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* ResumeButton;
        
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCommonButtonBase* RetreatButton;

private:
	UFUNCTION()
	void HandleResumeClicked();
    
	UFUNCTION()
	void HandleRetreatClicked();
};
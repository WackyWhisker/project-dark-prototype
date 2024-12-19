// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DkBaseMenuWidget.generated.h"

UCLASS(Abstract)
class DARK_API UDkBaseMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// Common UI already provides activation/deactivation through CommonActivatableWidget
	// We can add shared menu functionality here
    
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
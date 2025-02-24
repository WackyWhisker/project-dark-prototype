#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkScannableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDkScannableInterface : public UInterface
{
	GENERATED_BODY()
};

class DARK_API IDkScannableInterface
{
	GENERATED_BODY()

public:
	// Get scan configuration
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	EDkScanType GetScanType() const;
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	float GetScanDuration() const;
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	bool ShouldResetOnAbort() const;

	// Scan state management
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanModeEntered();
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanModeExited();
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanStart();
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanProgress(float Progress);
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanComplete();
    
	UFUNCTION(BlueprintNativeEvent, Category = "Scanning")
	void OnScanAborted();
};
// Copyright @ Christian Reichel

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkDeathHandlerInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDkDeathHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

class DARK_API IDkDeathHandlerInterface
{
	GENERATED_BODY()

public:
	// Declare the BlueprintNativeEvent function
	UFUNCTION(BlueprintNativeEvent, Category = "Death")
	void HandleDeath();
};
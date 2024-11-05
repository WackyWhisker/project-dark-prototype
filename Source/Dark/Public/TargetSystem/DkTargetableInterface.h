// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkTargetableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDkTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARK_API IDkTargetableInterface
{
	GENERATED_BODY()

public:
	// Returns whether this actor can currently be targeted
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	bool CanBeTargeted() const;

	// Called when this actor becomes the current target
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	void OnTargeted();

	// Called when this actor is no longer the current target
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	void OnUntargeted();

	// Retrieve the exact target location, if there is any different than the actor's location
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	FVector GetTargetLocation() const;
};

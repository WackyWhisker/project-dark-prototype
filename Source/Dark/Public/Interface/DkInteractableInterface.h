// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDkInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARK_API IDkInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Checks if the object can be interacted with
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(APawn* Interactor) const;

	// Called when interaction starts
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(APawn* Interactor);

	// Optional - Get the interaction prompt text
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractPrompt() const;
};

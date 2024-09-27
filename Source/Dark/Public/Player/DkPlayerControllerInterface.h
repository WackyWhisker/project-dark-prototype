// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkPlayerControllerInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FJumpSignature);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDkPlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARK_API IDkPlayerControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FJumpSignature* GetJumpDelegate() = 0;
};

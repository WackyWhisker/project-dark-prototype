// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DkPlayerControllerInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FJumpSignature);
DECLARE_MULTICAST_DELEGATE(FDodgeSignature);
DECLARE_MULTICAST_DELEGATE(FAttackSignature);
DECLARE_MULTICAST_DELEGATE(FTargetStartSignature);
DECLARE_MULTICAST_DELEGATE(FTargetEndSignature);
DECLARE_MULTICAST_DELEGATE(FTargetCycleLeftSignature);
DECLARE_MULTICAST_DELEGATE(FTargetCycleRightSignature);
DECLARE_MULTICAST_DELEGATE(FDropSignature);
DECLARE_MULTICAST_DELEGATE(FLiftSignature);


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
	virtual FDodgeSignature* GetDodgeDelegate() = 0;
	virtual FAttackSignature* GetAttackDelegate() = 0;
	virtual FTargetStartSignature* GetTargetStartDelegate() = 0;
	virtual FTargetEndSignature* GetTargetEndDelegate() = 0;
	virtual FTargetCycleLeftSignature* GetTargetCycleLeftDelegate() = 0;
	virtual FTargetCycleRightSignature* GetTargetCycleRightDelegate() = 0;
	virtual FDropSignature* GetDropDelegate() = 0;
	virtual FLiftSignature* GetLiftDelegate() = 0;
};
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
DECLARE_MULTICAST_DELEGATE(FTogglePauseMenuSignature);
DECLARE_MULTICAST_DELEGATE(FToggleUpgradeMenuSignature);
DECLARE_MULTICAST_DELEGATE(FInteractSignature);
DECLARE_MULTICAST_DELEGATE(FScanExecuteStartSignature);
DECLARE_MULTICAST_DELEGATE(FScanExecuteEndSignature);
DECLARE_MULTICAST_DELEGATE(FShootSignature);
DECLARE_MULTICAST_DELEGATE(FFocusStartSignature);
DECLARE_MULTICAST_DELEGATE(FFocusEndSignature);
DECLARE_MULTICAST_DELEGATE(FSwitchFocusModeSignature);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDkPlayerControllerInterface : public UInterface
{
    GENERATED_BODY()
};

class DARK_API IDkPlayerControllerInterface
{
    GENERATED_BODY()

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
    virtual FTogglePauseMenuSignature* GetTogglePauseMenuDelegate() = 0;
    virtual FToggleUpgradeMenuSignature* GetToggleUpgradeMenuDelegate() = 0;
    virtual FInteractSignature* GetInteractDelegate() = 0;
    virtual FScanExecuteStartSignature* GetScanExecuteStartDelegate() = 0;
    virtual FScanExecuteEndSignature* GetScanExecuteEndDelegate() = 0;
    virtual FShootSignature* GetShootDelegate() = 0;
    virtual FFocusStartSignature* GetFocusStartDelegate() = 0;
    virtual FFocusEndSignature* GetFocusEndDelegate() = 0;
    virtual FSwitchFocusModeSignature* GetSwitchFocusModeDelegate() = 0;
};
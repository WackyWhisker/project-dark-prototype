// Copyright @ Christian Reichel

#include "Subsystem/CastleSubsystem.h"

DEFINE_LOG_CATEGORY(CastleLog)

void UCastleSubsystem::TestSubsystemAccess()
{
	UE_LOG(CastleLog, Warning, TEXT("Castle Subsystem accessed successfully"));
}

// Copyright @ Christian Reichel


#include "Core/DkGameModeBase.h"

#include "Kismet/GameplayStatics.h"

void ADkGameModeBase::RestartLevelDelayed()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADkGameModeBase::LoadNewLevel, 1.5f, false);
}

void ADkGameModeBase::LoadNewLevel()
{
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(World->StreamingLevelsPrefix);
		UGameplayStatics::OpenLevel(World, FName(*CurrentLevel));
	}
}

// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DkImGuiSubsystem.generated.h"


UCLASS()
class DARK_API UDkImGuiSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override { return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always; }
	virtual bool IsTickableWhenPaused() const override { return true; }

	UFUNCTION(BlueprintCallable)
	static void ToggleImGuiInput();
};

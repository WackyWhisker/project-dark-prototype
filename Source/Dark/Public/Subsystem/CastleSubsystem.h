// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CastleSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CastleLog, Log, All);

UCLASS()
class DARK_API UCastleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//Public Methods -------------------
	
	// Simple function we can call to verify subsystem is accessible
	UFUNCTION(BlueprintCallable, Category = "Castle")
	void TestSubsystemAccess();

public:
	//Public Properties ---------------

protected:
	//Protected Methods ---------------

protected:
	//Protected Properties ------------

private:
	//Private Methods ----------------

private:
	//Private Properties -------------
};
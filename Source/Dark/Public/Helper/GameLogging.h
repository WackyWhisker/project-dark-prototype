// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(CastleLog, Log, All);

class DARK_API FGameLogger
{
public:
	static FGameLogger& Get();

	// Register a new logging category
	void RegisterLogCategory(const FString& CategoryName, const FString& Description);
	bool IsLoggingEnabled(const FString& CategoryName) const;

private:
	FGameLogger();
    
	// Store CVars and their descriptions
	TMap<FString, TAutoConsoleVariable<int32>*> LoggingCVars;
	TMap<FString, FString> CategoryDescriptions;

	// Console command handler
	void HandleLogCommand(const TArray<FString>& Args);
};

// Logging macro
#define CASTLE_LOG(Verbosity, Format, ...) \
if(FGameLogger::Get().IsLoggingEnabled("Castle")) \
UE_LOG(CastleLog, Verbosity, Format, ##__VA_ARGS__)
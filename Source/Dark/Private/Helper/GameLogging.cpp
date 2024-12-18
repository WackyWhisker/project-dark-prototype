// Copyright @ Christian Reichel

#include "Helper/GameLogging.h"

DEFINE_LOG_CATEGORY(CastleLog);

FGameLogger& FGameLogger::Get()
{
    static FGameLogger Instance;
    return Instance;
}

FGameLogger::FGameLogger()
{
    // Register console command for help/control
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("Log"),
        TEXT("Usage: Log <category> [0/1] or 'Log list' to show all categories"),
        FConsoleCommandWithArgsDelegate::CreateRaw(this, &FGameLogger::HandleLogCommand),
        ECVF_Default
    );

    // Register your categories
    RegisterLogCategory("Castle", "Castle generation and room system logs");
}

void FGameLogger::RegisterLogCategory(const FString& CategoryName, const FString& Description)
{
    FString CvarName = FString::Printf(TEXT("%s.LogEnabled"), *CategoryName);
    
    // Create new CVar for this category
    TAutoConsoleVariable<int32>* NewCVar = new TAutoConsoleVariable<int32>(
        *CvarName,
        1,  // Default enabled
        *FString::Printf(TEXT("%s (0=disabled, 1=enabled)"), *Description),
        ECVF_Default
    );

    LoggingCVars.Add(CategoryName, NewCVar);
    CategoryDescriptions.Add(CategoryName, Description);
}

void FGameLogger::HandleLogCommand(const TArray<FString>& Args)
{
    if (Args.Num() == 0 || (Args.Num() == 1 && Args[0].ToLower() == TEXT("list")))
    {
        // List all categories and their states
        UE_LOG(LogTemp, Log, TEXT("Available logging categories:"));
        for (const auto& Pair : LoggingCVars)
        {
            const FString& Category = Pair.Key;
            bool bEnabled = IsLoggingEnabled(Category);
            UE_LOG(LogTemp, Log, TEXT("  %s: %s (%s)"), 
                *Category, 
                *CategoryDescriptions[Category],
                bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
        }
        return;
    }

    const FString& Category = Args[0];
    if (!LoggingCVars.Contains(Category))
    {
        UE_LOG(LogTemp, Warning, TEXT("Unknown log category: %s"), *Category);
        return;
    }

    if (Args.Num() == 1)
    {
        // Toggle if no state specified
        bool bCurrentState = IsLoggingEnabled(Category);
        LoggingCVars[Category]->AsVariable()->Set(!bCurrentState ? TEXT("1") : TEXT("0"), ECVF_SetByConsole);
        UE_LOG(LogTemp, Log, TEXT("%s logging: %s"), *Category,
            !bCurrentState ? TEXT("Enabled") : TEXT("Disabled"));
    }
    else
    {
        // Set to specified state
        bool bNewState = FCString::Atoi(*Args[1]) != 0;
        LoggingCVars[Category]->AsVariable()->Set(bNewState ? TEXT("1") : TEXT("0"), ECVF_SetByConsole);
        UE_LOG(LogTemp, Log, TEXT("%s logging: %s"), *Category,
            bNewState ? TEXT("Enabled") : TEXT("Disabled"));
    }
}

bool FGameLogger::IsLoggingEnabled(const FString& CategoryName) const
{
    if (const TAutoConsoleVariable<int32>* CVar = LoggingCVars.FindRef(CategoryName))
    {
        return CVar->GetValueOnGameThread() != 0;
    }
    return false;
}
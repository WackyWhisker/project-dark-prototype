// Copyright @ Christian Reichel

using UnrealBuildTool;

public class Dark : ModuleRules
{
    public Dark(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "EnhancedInput", 
            "UMG" 
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });
    }
}
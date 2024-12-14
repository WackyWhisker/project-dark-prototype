// Copyright @ Christian Reichel

using UnrealBuildTool;

public class DarkEditor : ModuleRules
{
	public DarkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"UnrealEd",
			"GraphEditor",
			"Slate", 
			"SlateCore",
			"ToolMenus",
			"KismetWidgets",
			"AssetTools",
			"ApplicationCore",
			"InputCore"  
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Dark"  // Reference to main game module
		});
	}
}
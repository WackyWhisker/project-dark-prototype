// Copyright @ Christian Reichel

using UnrealBuildTool;
using System.Collections.Generic;

public class DarkTarget : TargetRules
{
	public DarkTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Dark" } );
	}
}

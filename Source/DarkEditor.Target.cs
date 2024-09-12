// Copyright @ Christian Reichel

using UnrealBuildTool;
using System.Collections.Generic;

public class DarkEditorTarget : TargetRules
{
	public DarkEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Dark" } );
	}
}

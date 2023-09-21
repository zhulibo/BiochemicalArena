using UnrealBuildTool;
using System.Collections.Generic;

public class BiochemicalArenaTarget : TargetRules
{
	public BiochemicalArenaTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("BiochemicalArena");
	}
}

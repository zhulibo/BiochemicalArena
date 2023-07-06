using UnrealBuildTool;
using System.Collections.Generic;

public class BiochemicalArenaTarget : TargetRules
{
	public BiochemicalArenaTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("BiochemicalArena");
	}
}

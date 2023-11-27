using UnrealBuildTool;

public class BiochemicalArena : ModuleRules
{
	public BiochemicalArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"EnhancedInput",
				"Slate",
				"SlateCore",
				"UMG",
				"CommonUI",
			}
		);

	}
}

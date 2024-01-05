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
				"EnhancedInput",
				"OnlineServicesInterface",
				"CoreOnline",
				"Niagara",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore",
				"UMG",
				"CommonUI",
			}
		);

	}
}

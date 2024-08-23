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
				"CommonInput",
				"OnlineServicesInterface",
				"CoreOnline",
				"Niagara",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"DataRegistry",
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

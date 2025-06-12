// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MutateArena : ModuleRules
{
	public MutateArena(ReadOnlyTargetRules Target) : base(Target)
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
				"CommonInput",
				"Niagara",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"DataRegistry",
				"MetasoundEngine",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore",
				"UMG",
				"CommonUI",
				"RHI",
			}
		);

	}
}

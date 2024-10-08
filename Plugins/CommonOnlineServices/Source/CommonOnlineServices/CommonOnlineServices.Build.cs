// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class CommonOnlineServices : ModuleRules
{
	public CommonOnlineServices(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EOSSDK",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


		string BaseDir = Path.Combine("$(EngineDir)", "Source/ThirdParty/EOSSDK/SDK");
		if(Target.Platform == UnrealTargetPlatform.Win64)
		{
			// PublicIncludePaths.Add(Path.Combine(BaseDir, "Include"));

			// PublicAdditionalLibraries.Add(Path.Combine(BaseDir, "Lib/EOSSDK-Win64-Shipping.lib"));

			RuntimeDependencies.Add("$(BinaryOutputDir)/EOSSDK-Win64-Shipping.dll", Path.Combine(BaseDir, "Bin/EOSSDK-Win64-Shipping.dll"));
		}

	}
}

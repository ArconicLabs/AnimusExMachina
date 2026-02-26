// Copyright ArconicLabs. All Rights Reserved.

using UnrealBuildTool;

public class AnimusExMachina : ModuleRules
{
	public AnimusExMachina(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AIModule",
				"StateTreeModule",
				"GameplayStateTreeModule",
				"GameplayTags",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"NavigationSystem",
			}
		);
	}
}

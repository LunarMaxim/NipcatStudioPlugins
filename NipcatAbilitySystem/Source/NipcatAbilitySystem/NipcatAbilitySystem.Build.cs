// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NipcatAbilitySystem : ModuleRules
{
	public NipcatAbilitySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AIModule",
				"Core",
				"DeveloperSettings",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"MotionWarping",
				"PhysicsCore",
				"Projects",
				"NipcatBasicGameplay",
				"Chooser",
				"ProxyTable"
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
				"NetCore",
				"PhysicsCore", 
				"EnhancedInput", 
				"WorldConditions"
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"UnrealEd",
				"AnimationBlueprintLibrary",
				"NipcatAbilitySystemEditor"
			});
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NipcatBasicGameplay : ModuleRules
{
	public NipcatBasicGameplay(ReadOnlyTargetRules Target) : base(Target)
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
				"NipcatInput"
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
				"Projects",
				"GameplayTags",
				"GameplayAbilities",
				"AIModule",
				"EnhancedInput",
				"GameplayTasks",
				"DeveloperSettings", 
				"MotionWarping", 
				"AnimGraphRuntime",
				"GeometryCollectionEngine", 
				"AnimationLocomotionLibraryRuntime", 
				"IKRig", 
				"Chooser", 
				"TargetingSystem",
				"ModelViewViewModel",
				"AsyncMixin",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);

		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"Persona",
				"UnrealEd",
				"AssetTools"
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

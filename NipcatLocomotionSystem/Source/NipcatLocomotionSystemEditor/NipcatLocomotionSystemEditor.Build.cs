using UnrealBuildTool;

public class NipcatLocomotionSystemEditor : ModuleRules
{
    public NipcatLocomotionSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "KismetCompiler",
                "UnrealEd",
                "BlueprintGraph",
                "GraphEditor",
                "AnimGraph",
                "AnimationModifiers",
                "AnimationBlueprintLibrary",
                "AnimGraphRuntime",
                "Kismet",
                "NipcatBasicGameplay",
                "AnimationModifierLibrary"
            }
        );
    }
}
using UnrealBuildTool;

public class NipcatBasicGameplayEditor : ModuleRules
{
    public NipcatBasicGameplayEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "UnrealEd",
                "StaticMeshEditor",
                "DeveloperSettings",
                "GameplayTags",
                "Blutility", 
                "NipcatBasicGameplay", 
                "IKRigEditor", 
                "IKRig",
                "ToolMenus",
                "PropertyEditor",
                "UMGEditor" 
            }
        );
    }
}
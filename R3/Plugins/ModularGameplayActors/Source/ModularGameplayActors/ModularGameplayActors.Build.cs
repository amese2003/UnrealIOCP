
using UnrealBuildTool;
using System.IO;

public class ModularGameplayActors : ModuleRules
{
    public ModularGameplayActors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {

            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {

            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "ModularGameplay",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {

            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {

            }
        );
    }
}
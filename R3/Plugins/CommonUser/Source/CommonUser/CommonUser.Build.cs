using UnrealBuildTool;

public class CommonUser : ModuleRules
{
    public CommonUser(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                // ... and other public dependencies that you statically link with here ...
            }
        );
    }
}
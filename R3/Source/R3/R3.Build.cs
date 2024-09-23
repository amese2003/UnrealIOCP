// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class R3 : ModuleRules
{
	public R3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", 
            "CoreUObject",
            "Engine",
            "InputCore",  
			// GAS
			"GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
			// GameFeatures
			"ModularGameplay",
            "GameFeatures",
            "ModularGameplayActors",
			// Input
			"InputCore",
            "EnhancedInput",
			// CommonUser
			"CommonUser",
			// CommonGame
			"CommonGame",
			// UMG
			"UMG",

			// SocketNetwork
			"Sockets", 
            "Networking",
			// Slate
			"Slate",
            "SlateCore",
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "R3/Network/",
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });



        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}

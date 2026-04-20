// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Motorfest_HUD_multi : ModuleRules
{
	public Motorfest_HUD_multi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Motorfest_HUD_multi",
			"Motorfest_HUD_multi/Variant_Platforming",
			"Motorfest_HUD_multi/Variant_Platforming/Animation",
			"Motorfest_HUD_multi/Variant_Combat",
			"Motorfest_HUD_multi/Variant_Combat/AI",
			"Motorfest_HUD_multi/Variant_Combat/Animation",
			"Motorfest_HUD_multi/Variant_Combat/Gameplay",
			"Motorfest_HUD_multi/Variant_Combat/Interfaces",
			"Motorfest_HUD_multi/Variant_Combat/UI",
			"Motorfest_HUD_multi/Variant_SideScrolling",
			"Motorfest_HUD_multi/Variant_SideScrolling/AI",
			"Motorfest_HUD_multi/Variant_SideScrolling/Gameplay",
			"Motorfest_HUD_multi/Variant_SideScrolling/Interfaces",
			"Motorfest_HUD_multi/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

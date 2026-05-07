// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlatformDad : ModuleRules
{
	public PlatformDad(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(["Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "SlateCore", "Slate" , "Niagara"]);
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlatformDadClientTarget : TargetRules
{
	public PlatformDadClientTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("PlatformDad");
	}
}

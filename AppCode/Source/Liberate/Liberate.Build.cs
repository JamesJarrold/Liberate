// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Liberate : ModuleRules
{
	public Liberate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "NavigationSystem", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });
	}
}

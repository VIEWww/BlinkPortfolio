// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlinkPortfolio : ModuleRules
{
	public BlinkPortfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", 
	        "CoreUObject", 
	        "Engine", 
	        "InputCore", 
	        "HeadMountedDisplay", 
	        "NavigationSystem",
	        "AIModule", 
	        "Niagara", 
	        "EnhancedInput",
	        "UMG",
	        "Slate",
	        "SlateCore",
	        "CommonUI",
	        
	        "BlinkTechPlugin",
	        "UIView",
        });
    }
}

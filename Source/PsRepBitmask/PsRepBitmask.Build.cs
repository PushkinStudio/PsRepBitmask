// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
    public class PsRepBitmask : ModuleRules
    {
        public PsRepBitmask(ReadOnlyTargetRules Target) : base(Target)
        {
            PrivateIncludePaths.AddRange(
                new string[] {
                    "PsRepBitmask/Private",
                });

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine"
                });
        }
    }
}
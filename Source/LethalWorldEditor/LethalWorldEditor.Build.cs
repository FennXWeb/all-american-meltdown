using System.IO;
using UnrealBuildTool;

public class LethalWorldEditor : ModuleRules
{
    public LethalWorldEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.NoPCHs;
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "LethalWorld", "UnrealEd",
            "AssetTools", "ToolMenus", "Slate", "SlateCore"
        });
        // The existing runtime module keeps its exported headers at its root.
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../LethalWorld"));
    }
}

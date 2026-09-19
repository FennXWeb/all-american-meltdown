using UnrealBuildTool;
public class LethalWorldEditorTarget : TargetRules
{
    public LethalWorldEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("LethalWorld");
        ExtraModuleNames.Add("LethalWorldEditor");
    }
}

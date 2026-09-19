using UnrealBuildTool;
public class LethalWorldTarget : TargetRules
{
    public LethalWorldTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("LethalWorld");
    }
}

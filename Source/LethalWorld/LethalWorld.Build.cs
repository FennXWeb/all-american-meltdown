using UnrealBuildTool;
public class LethalWorld : ModuleRules
{
    public LethalWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.NoPCHs;
        bUseUnity = true;
        MinSourceFilesForUnityBuildOverride = 1;
        PrivateIncludePaths.Add(ModuleDirectory);
        RuntimeDependencies.Add("$(ProjectDir)/Content/UI55/T_Menu55.png", StagedFileType.UFS);
        PrivateDependencyModuleNames.AddRange(new string[]{"RenderCore","RHI","MoviePlayer","Slate","SlateCore"});
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "PhysicsCore" });
    }
}


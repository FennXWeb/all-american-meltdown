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
        foreach (string LoadingFile in System.IO.Directory.GetFiles(System.IO.Path.Combine(ModuleDirectory, "../../Content/Loading79")))
            RuntimeDependencies.Add(LoadingFile, StagedFileType.UFS);
        PrivateDependencyModuleNames.AddRange(new string[]{"ApplicationCore","RenderCore","RHI","MoviePlayer","Slate","SlateCore","DLSSBlueprint","StreamlineBlueprint","StreamlineDLSSGBlueprint","StreamlineReflexBlueprint"});
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "PhysicsCore" });
    }
}


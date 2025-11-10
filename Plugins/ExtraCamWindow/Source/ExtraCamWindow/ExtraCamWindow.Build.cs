using UnrealBuildTool;

public class ExtraCamWindow : ModuleRules
{
    public ExtraCamWindow(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "CinematicCamera", "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore", "UMG" });
        PrivateDependencyModuleNames.AddRange(new string[] { "CineCameraSceneCapture" });

        //Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        //if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        //{
        //    if (UEBuildConfiguration.bCompileSteamOSS == true)
        //    {
        //        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //    }
        //}
    }
}

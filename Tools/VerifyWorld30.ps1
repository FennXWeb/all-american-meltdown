$ErrorActionPreference='Stop'
$editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project='X:/LethalWorld/LethalWorld.uproject'
$world30Failures=@()
foreach($suite in @('Vehicle30','POI30','CompanionNav')){
 $log="X:/LethalWorld/Saved/World30_$suite.log"
 $run=Start-Process $editor -ArgumentList @($project,'/Game/Maps/Wasteland','-game','-LWV17Smoke',"-LW${suite}Smoke",'-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
 if($run.ExitCode -ne 0 -or !(Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE failures=0' -Quiet)){$world30Failures+=$suite;Write-Output "$suite FAILED; inspect $log"}else{Write-Output "$suite passed"}
}
if($world30Failures.Count){throw ("Failed suites: "+($world30Failures -join ", "))}

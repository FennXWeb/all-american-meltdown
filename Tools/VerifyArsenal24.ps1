$ErrorActionPreference='Stop'
$editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project='X:/LethalWorld/LethalWorld.uproject'
$log='X:/LethalWorld/Saved/Arsenal24_Automation.log'
$p=Start-Process $editor -ArgumentList @($project,'-unattended','-nop4','-nullrhi','-nosplash','-ExecCmds="Automation RunTests LethalWorld"','-TestExit="Automation Test Queue Empty"','-ReportExportPath=X:/LethalWorld/Saved/Arsenal24Report',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or (Select-String -LiteralPath $log -SimpleMatch 'Result={Fail}' -Quiet) -or (Select-String -LiteralPath $log -SimpleMatch 'Result={Success}').Count -lt 50){throw 'Automation failed'}
$log='X:/LethalWorld/Saved/Arsenal24_Runtime.log'
$p=Start-Process $editor -ArgumentList @($project,'/Game/Maps/Wasteland','-game','-LWV17Smoke','-LWArsenalSmoke','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE failures=0' -Quiet)){throw 'Arsenal runtime checks failed'}
Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE'

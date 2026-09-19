$ErrorActionPreference='Stop'
$editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project='X:/LethalWorld/LethalWorld.uproject'
$log='X:/LethalWorld/Saved/Combat29_Automation.log'
$p=Start-Process $editor -ArgumentList @($project,'-unattended','-nop4','-nullrhi','-nosplash','-ExecCmds="Automation RunTests LethalWorld"','-TestExit="Automation Test Queue Empty"','-ReportExportPath=X:/LethalWorld/Saved/Combat29Report',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or (Select-String -LiteralPath $log -SimpleMatch 'Result={Fail}' -Quiet) -or (Select-String -LiteralPath $log -SimpleMatch 'Result={Success}').Count -lt 61){throw 'Automation failed'}
$log='X:/LethalWorld/Saved/Combat29_Runtime.log'
$p=Start-Process $editor -ArgumentList @($project,'/Game/Maps/Wasteland','-game','-LWV17Smoke','-LWCombat29Smoke','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE failures=0' -Quiet)){throw 'Combat runtime checks failed'}
Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE'


$ErrorActionPreference='Stop'
$editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project='X:/LethalWorld/LethalWorld.uproject'
$log='X:/LethalWorld/Saved/Casino21_Content.log'
$p=Start-Process $editor -ArgumentList @($project,'-run=pythonscript','-script=X:/LethalWorld/Tools/build_v21_content.py','-unattended','-nop4','-nullrhi','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $log -SimpleMatch 'AAM_CASINO21_CONTENT_COMPLETE' -Quiet)){throw 'Casino content import failed'}
$log='X:/LethalWorld/Saved/Casino21_Automation.log'
$p=Start-Process $editor -ArgumentList @($project,'-unattended','-nop4','-nullrhi','-nosplash','-ExecCmds="Automation RunTests LethalWorld"','-TestExit="Automation Test Queue Empty"',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or (Select-String -LiteralPath $log -SimpleMatch 'Result={Fail}' -Quiet)){throw 'Casino automation failed'}
$log='X:/LethalWorld/Saved/Casino21_Rendered.log'
$p=Start-Process $editor -ArgumentList @($project,'/Game/Maps/Wasteland','-game','-LWV17Smoke','-LWCasinoSmoke','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE failures=0' -Quiet)){throw 'Casino rendered checks failed'}
Select-String -LiteralPath $log -SimpleMatch 'AAM_V17_DONE'

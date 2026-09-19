$ErrorActionPreference='Stop'
$editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project='X:/LethalWorld/LethalWorld.uproject'
$importLog='X:/LethalWorld/Saved/Urban20_Content.log'
$p=Start-Process $editor -ArgumentList @($project,'-run=pythonscript','-script=X:/LethalWorld/Tools/build_urban20_content.py','-unattended','-nop4','-nullrhi','-nosplash',"-abslog=$importLog") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $importLog -SimpleMatch 'AAM_URBAN20_CONTENT_COMPLETE' -Quiet)){throw 'Urban content import failed'}
$testLog='X:/LethalWorld/Saved/Urban20_Automation.log'
$p=Start-Process $editor -ArgumentList @($project,'-unattended','-nop4','-nullrhi','-nosplash','-ExecCmds="Automation RunTests LethalWorld"','-TestExit="Automation Test Queue Empty"',"-abslog=$testLog") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or (Select-String -LiteralPath $testLog -SimpleMatch "Result={Fail}" -Quiet)){throw "Urban automation failed"}
Write-Output "Automation exit: $($p.ExitCode)"
Select-String -LiteralPath $testLog -Pattern 'Result=|Cities=|Test Completed' | Select-Object -Last 55
$smokeLog='X:/LethalWorld/Saved/Urban20_Rendered.log'
$p=Start-Process $editor -ArgumentList @($project,'/Game/Maps/Wasteland','-game','-LWV17Smoke','-LWUrbanSmoke','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$smokeLog") -WindowStyle Hidden -Wait -PassThru
if($p.ExitCode -ne 0 -or !(Select-String -LiteralPath $smokeLog -SimpleMatch 'AAM_V17_DONE failures=0' -Quiet)){throw 'Urban rendered checks failed'}
Select-String -LiteralPath $smokeLog -Pattern 'AAM_V17_DONE|URBAN20 downtown'

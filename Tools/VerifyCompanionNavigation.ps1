param([string]$Engine='G:/Epic/UE_5.8')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
foreach($suite in @('CompanionNav','V18','UIClick')){
 $flag=if($suite -eq 'CompanionNav'){'-LWCompanionNavSmoke'}elseif($suite -eq 'V18'){'-LWV18Smoke'}else{'-LWUIClickSmoke'}
 $log=Join-Path $root "Saved/Navigation19_$suite.log"
 $run=Start-Process $editor -ArgumentList @("$root/LethalWorld.uproject",'/Game/Maps/Wasteland','-game','-LWV17Smoke',$flag,'-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$log") -WindowStyle Hidden -Wait -PassThru
 if($run.ExitCode -ne 0){throw "$suite exited with $($run.ExitCode)"}
 $text=Get-Content -LiteralPath $log -Raw
 if($text -notmatch 'AAM_V17_DONE failures=0'){throw "$suite has failed checks; inspect $log"}
 Write-Output "$suite passed"
}
Write-Output 'Navigation, convoy/gameplay and UI checks passed. No packaging was run.'

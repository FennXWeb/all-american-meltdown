param([string]$Engine='G:/Epic/UE_5.8')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path $PSScriptRoot -Parent
$editor=Join-Path $Engine 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$project=Join-Path $projectRoot 'LethalWorld.uproject'
function Run-EditorCheck([string[]]$Arguments,[string]$Log,[string]$Marker){
 $result=Start-Process -FilePath $editor -ArgumentList (@($project)+$Arguments+@('-unattended','-nop4',"-abslog=$projectRoot/Saved/$Log")) -WindowStyle Hidden -Wait -PassThru
 if($result.ExitCode -ne 0){throw "$Log exited with $($result.ExitCode)"}
 $text=Get-Content -LiteralPath "$projectRoot/Saved/$Log" -Raw
 if($text -notmatch $Marker){throw "$Log did not report success; inspect its checks."}
 Write-Output "$Log passed"
}
Run-EditorCheck @('-nullrhi','-ExecCmds="Automation RunTests LethalWorld"','-TestExit="Automation Test Queue Empty"') 'Automation18.log' 'Test Completed. Result=\{Success\}'
$testText=Get-Content "$projectRoot/Saved/Automation18.log" -Raw
if($testText -match 'Test Completed. Result=\{Fail'){throw 'Automation test failed.'}
$passed=([regex]::Matches($testText,'Test Completed. Result=\{Success\}')).Count
if($passed -lt 45){throw "Expected at least 45 automation tests, got $passed"}
$render=@('/Game/Maps/Wasteland','-game','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-nosplash')
Run-EditorCheck ($render+@('-LWV17Smoke','-LWV18Smoke')) 'Gameplay18.log' 'AAM_V17_DONE failures=0'
Run-EditorCheck ($render+@('-LWV17Smoke','-LWUIClickSmoke')) 'UIClick18.log' 'AAM_V17_DONE failures=0'
Run-EditorCheck ($render+@('-LWV14Smoke')) 'Regression18_V14.log' 'LW_V14_DONE failures=0'
Write-Output "$passed automation tests and three rendered suites passed. No packaging was run."

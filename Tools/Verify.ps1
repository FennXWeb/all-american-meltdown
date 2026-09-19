param([string]$Engine='G:\Epic\UE_5.8')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
$exe=Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$project=Join-Path $projectRoot 'LethalWorld.uproject'
& $exe $project -unattended -nop4 -nullrhi '-ExecCmds=Automation RunTests LethalWorld' '-TestExit=Automation Test Queue Empty' "-abslog=$projectRoot\Saved\AutomationV3.log"
if($LASTEXITCODE -ne 0){throw 'Automation tests failed.'}
$automation=Get-Content -LiteralPath "$projectRoot\Saved\AutomationV3.log" -Raw
if($automation -match 'Result=\{Fail\}|Result=\{Error\}'){throw 'An automation test failed; inspect Saved/AutomationV3.log.'}
$passed=([regex]::Matches($automation,'Test Completed\. Result=\{Success\}')).Count
if($passed -lt 45){throw "Only $passed automation tests completed; expected at least 45."}
& $exe $project /Game/Maps/Wasteland -game -LWV2Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV2.log"
if($LASTEXITCODE -ne 0){throw 'Gameplay test process failed.'}
$log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV2.log" -Raw
if($log -notmatch 'LW_V2_DONE failures=0'){throw 'Gameplay checks did not complete successfully; inspect Saved/GameplayV2.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV3Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV3.log"
if($LASTEXITCODE -ne 0){throw 'V3 gameplay test process failed.'}
$v3log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV3.log" -Raw
if($v3log -notmatch 'LW_V3_DONE failures=0'){throw 'V3 checks did not complete successfully; inspect Saved/GameplayV3.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV4Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV4.log"
if($LASTEXITCODE -ne 0){throw 'V4 gameplay test process failed.'}
$v4log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV4.log" -Raw
if($v4log -notmatch 'LW_V4_DONE failures=0'){throw 'V4 checks did not complete successfully; inspect Saved/GameplayV4.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV5Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV5.log"
if($LASTEXITCODE -ne 0){throw 'V5 gameplay test process failed.'}
$v5log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV5.log" -Raw
if($v5log -notmatch 'LW_V5_DONE failures=0'){throw 'V5 checks did not complete successfully; inspect Saved/GameplayV5.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV6Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV6.log"
if($LASTEXITCODE -ne 0){throw 'V6 gameplay test process failed.'}
$v6log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV6.log" -Raw
if($v6log -notmatch 'LW_V6_DONE failures=0'){throw 'V6 checks failed; inspect Saved/GameplayV6.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV9Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV9.log"
if($LASTEXITCODE -ne 0){throw 'V9 gameplay process failed.'}
$v9log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV9.log" -Raw
if($v9log -notmatch 'LW_V9_DONE failures=0'){throw 'V9 checks failed; inspect Saved/GameplayV9.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV10Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV10.log"
if($LASTEXITCODE -ne 0){throw 'V10 gameplay process failed.'}
$v10log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV10.log" -Raw
if($v10log -notmatch 'LW_V10_DONE failures=0'){throw 'V10 checks failed; inspect Saved/GameplayV10.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV11Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV11.log"
if($LASTEXITCODE -ne 0){throw 'V11 gameplay process failed.'}
$v11log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV11.log" -Raw
if($v11log -notmatch 'LW_V11_DONE failures=0'){throw 'V11 checks failed; inspect Saved/GameplayV11.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV14Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV14.log"
if($LASTEXITCODE -ne 0){throw 'V14 gameplay process failed.'}
$v14log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV14.log" -Raw
if($v14log -notmatch 'LW_V14_DONE failures=0'){throw 'V14 checks failed; inspect Saved/GameplayV14.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV15Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV15.log"
if($LASTEXITCODE -ne 0){throw 'V15 gameplay process failed.'}
$v15log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV15.log" -Raw
if($v15log -notmatch 'AAM_V15_DONE failures=0'){throw 'V15 checks failed; inspect Saved/GameplayV15.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV16Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV16.log"
if($LASTEXITCODE -ne 0){throw 'V16 gameplay process failed.'}
$v16log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV16.log" -Raw
if($v16log -notmatch 'AAM_V16_DONE failures=0'){throw 'V16 checks failed; inspect Saved/GameplayV16.log.'}
& $exe $project /Game/Maps/Wasteland -game -LWV17Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 "-abslog=$projectRoot\Saved\GameplayV17.log"
if($LASTEXITCODE -ne 0){throw 'V17 gameplay process failed.'}
$v17log=Get-Content -LiteralPath "$projectRoot\Saved\GameplayV17.log" -Raw
if($v17log -notmatch 'AAM_V17_DONE failures=0'){throw 'V17 checks failed; inspect Saved/GameplayV17.log.'}
Write-Output "$passed automation tests and all gameplay suites passed."

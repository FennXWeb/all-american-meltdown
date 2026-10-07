param([string]$Editor='G:\Epic\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe')
$projectPath=Join-Path (Split-Path $PSScriptRoot -Parent) 'LethalWorld.uproject'
if (!(Test-Path -LiteralPath $Editor)) { throw 'Pass -Editor with your UnrealEditor.exe path.' }
# Runs the project directly. Does not cook or package a build.
& $Editor $projectPath -game -dx11 -LWLowSpec58 -windowed -ResX=1280 -ResY=720

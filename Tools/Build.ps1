param(
    [string]$Engine = 'G:\Epic\UE_5.8',
    [switch]$Content,
    [switch]$RebuildOriginalContent,
    [switch]$Package
)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
$project=Join-Path $projectRoot 'LethalWorld.uproject'
$dotnet=Join-Path $Engine 'Engine\Binaries\ThirdParty\DotNet\10.0\win-x64\dotnet.exe'
$ubt=Join-Path $Engine 'Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll'
& $dotnet $ubt LethalWorldEditor Win64 Development "-Project=$project" -WaitMutex -NoHotReloadFromIDE
if($LASTEXITCODE -ne 0){throw 'Unreal C++ compilation failed.'}
if($RebuildOriginalContent){
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentBuild.log"
    if($LASTEXITCODE -ne 0){throw 'Native content build failed.'}
}
if($Content -or $RebuildOriginalContent){
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v2_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV2.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.2 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v3_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV3.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.3 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v4_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV4.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.4 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v5_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV5.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.5 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v7_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV7.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.7 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v8_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV8.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.8 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v9_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV9.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.9 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v10_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV10.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.10 content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v12_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV12.log"
    if($LASTEXITCODE -ne 0){throw 'Version 0.12 card content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v13_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV13.log"
    if($LASTEXITCODE -ne 0){throw 'POI furniture content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v14_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV14.log"
    if($LASTEXITCODE -ne 0){throw 'Camper and equipment content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v15_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV15.log"
    if($LASTEXITCODE -ne 0){throw 'Opening and branding content import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v16_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV16.log"
    if($LASTEXITCODE -ne 0){throw 'Survivor, coach and transparent branding import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v17_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV17.log"
    if($LASTEXITCODE -ne 0){throw 'Motorhome access, hand and driving audio import failed.'}
    & (Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') $project -run=pythonscript "-script=$PSScriptRoot\build_v18_content.py" -unattended -nop4 -nullrhi "-abslog=$projectRoot\Saved\ContentV18.log"
    if($LASTEXITCODE -ne 0){throw 'Settlement, creature, POI and double barrel content import failed.'}
}
if($Package){
    & (Join-Path $Engine 'Engine\Build\BatchFiles\RunUAT.bat') BuildCookRun "-project=$project" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$projectRoot\Builds" -unattended -utf8output
    if($LASTEXITCODE -ne 0){throw 'Windows packaging failed.'}
}

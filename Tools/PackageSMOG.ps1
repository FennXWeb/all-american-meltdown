param(
    [string]$Engine = 'G:\Epic\UE_5.8',
    [string]$Version = '0.84.0',
    [switch]$ReuseCook
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$project = Join-Path $projectRoot 'LethalWorld.uproject'
$archive = Join-Path $projectRoot "Builds/SMOG-$Version"
$uat = Join-Path $Engine 'Engine/Build/BatchFiles/RunUAT.bat'
$appLocal = Join-Path $Engine 'Engine/Binaries/ThirdParty/AppLocalDependencies'
[string[]]$cookArgs = @('-cook','-cookall')
if ($ReuseCook) { $cookArgs = @('-skipcook') }
& $uat BuildCookRun "-project=$project" -noP4 -platform=Win64 -clientconfig=Shipping -build @cookArgs -stage -pak -iostore -compressed -archive "-archivedirectory=$archive" "-applocaldirectory=$appLocal" -prereqs -nodebuginfo -unattended -utf8output
if ($LASTEXITCODE -ne 0) { throw 'Windows Shipping packaging failed.' }
$build = Join-Path $archive 'Windows'
if (!(Test-Path -LiteralPath (Join-Path $build 'LethalWorld/Binaries/Win64/LethalWorld-Win64-Shipping.exe'))) { throw 'Packaged game executable missing.' }
foreach ($name in @('smog_meta.xml','smog_launch.bat','smog_header.png','smog_logo.png','smog_icon.ico')) {
    Copy-Item -LiteralPath (Join-Path $projectRoot $name) -Destination (Join-Path $build $name) -Force
}
$notices = Join-Path $build 'Notices'
New-Item -ItemType Directory -Path $notices -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $projectRoot 'Docs/Geography84-Sources.md') -Destination (Join-Path $notices 'Geography-and-Map-Attribution.md') -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'Plugins/DLSS/Source/ThirdParty/NGX/LICENSE.txt') -Destination (Join-Path $notices 'NVIDIA-RTX-SDK-LICENSE.txt') -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'Docs/SMOG/Release.md') -Destination (Join-Path $build 'README.md') -Force
Write-Output "SMOG stage ready: $build"

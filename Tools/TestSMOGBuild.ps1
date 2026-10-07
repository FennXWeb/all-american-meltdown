param(
    [string]$BuildDirectory,
    [ValidateSet('LWMenu81Smoke','LWUpdate84Smoke')][string]$Suite = 'LWMenu81Smoke'
)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$build=(Resolve-Path -LiteralPath $BuildDirectory).Path
$profile=Join-Path $root "Saved/Publishing84/Profiles/$Suite"
if(Test-Path -LiteralPath $profile){throw "Test profile already exists; use a fresh profile for this test: $profile"}
New-Item -ItemType Directory -Path $profile -Force | Out-Null
$start=[Diagnostics.ProcessStartInfo]::new()
$start.FileName=$env:ComSpec
$start.WorkingDirectory=$build
$start.Arguments='/d /c ""'+(Join-Path $build 'smog_launch.bat')+'" -unattended -nosplash -nosound -windowed -ForceRes -ResX=1920 -ResY=1080 -LWV17Smoke -LWUI46Smoke -'+$Suite+'"'
$start.UseShellExecute=$false
$start.CreateNoWindow=$true
$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
# Change only this child process's environment. Never touch the player's real saves.
$start.EnvironmentVariables['LOCALAPPDATA']=$profile
$process=[Diagnostics.Process]::Start($start)
if(!$process.WaitForExit(600000)){throw "Smoke test exceeded ten minutes; inspect owned test process $($process.Id)."}
$report=Join-Path $profile 'AllAmericanMeltdown/Saved/SmokeResult.json'
if(!(Test-Path -LiteralPath $report)){throw "Shipping test produced no result (exit $($process.ExitCode))."}
$result=Get-Content -LiteralPath $report -Raw | ConvertFrom-Json
if($process.ExitCode -ne 0 -or $result.failures -ne 0 -or $result.checks -le 0){throw "Shipping smoke failed: $(Get-Content -LiteralPath $report -Raw)"}
if(Test-Path -LiteralPath (Join-Path $build 'LethalWorld/Saved/SaveGames')){throw 'Save files appeared inside the versioned installation.'}
Write-Output "$Suite PASS: $($result.checks) checks, $($result.seconds) seconds; launcher waited and exited successfully."
Write-Output "Isolated user data: $profile"

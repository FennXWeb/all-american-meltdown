$ErrorActionPreference='Stop'
# Runtime only: run after the coordinator builds the editor target.
$vehicle30Editor='G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$vehicle30Project='X:/LethalWorld/LethalWorld.uproject'
$vehicle30Log='X:/LethalWorld/Saved/Vehicle30_Runtime.log'
$vehicle30Process=Start-Process $vehicle30Editor -ArgumentList @($vehicle30Project,'/Game/Maps/Wasteland','-game','-LWV17Smoke','-LWVehicle30Smoke','-LWAudioSmoke','-RenderOffscreen','-windowed','-ResX=1280','-ResY=720','-unattended','-nop4','-nosplash',"-abslog=$vehicle30Log") -WindowStyle Hidden -PassThru
if(!$vehicle30Process.WaitForExit(240000)){
    Stop-Process -Id $vehicle30Process.Id
    throw 'Vehicle30 runtime exceeded 240 seconds'
}
$vehicle30Process.Refresh()
if($vehicle30Process.ExitCode -ne 0 -or !(Test-Path -LiteralPath $vehicle30Log) -or !(Select-String -LiteralPath $vehicle30Log -Pattern 'LW_VEHICLE30_DONE failures=0 checks=[1-9][0-9]*' -Quiet)){
    throw "Vehicle30 runtime failed; inspect $vehicle30Log"
}
Select-String -LiteralPath $vehicle30Log -SimpleMatch 'LW_VEHICLE30_DONE'

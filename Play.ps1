param([string]$Engine='G:\Epic\UE_5.8')
$project=Join-Path $PSScriptRoot 'LethalWorld.uproject'
$packaged=Join-Path $PSScriptRoot 'Builds\Windows\LethalWorld.exe'
if(Test-Path -LiteralPath $packaged){Start-Process -FilePath $packaged -WorkingDirectory (Split-Path $packaged)}
else {
    $editor=Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor.exe'
    if(!(Test-Path -LiteralPath $editor)){throw 'Set -Engine to your Unreal Engine 5.8 installation.'}
    Start-Process -FilePath $editor -ArgumentList @($project,'-game','-windowed','-ResX=1600','-ResY=900','-NoSplash') -WorkingDirectory $PSScriptRoot
}

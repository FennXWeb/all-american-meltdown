$projectRoot=Split-Path $PSScriptRoot -Parent
$cachePath=Join-Path $projectRoot 'Saved/Vendor58'
New-Item -ItemType Directory -Force $cachePath | Out-Null
$archivePath=Join-Path $cachePath 'DLSS58.zip'
$url='https://developer.nvidia.com/downloads/assets/gameworks/downloads/secure/dlss/ue-dlss-5.8/8.8.0/2026.09.15_ue5.8_dlss4.5plugin_v8.8.0.zip'
$expected='05830BA47E5A34C402702DB127D5876E024C52583F34F69582A6C6CF68E76E0A'
if (!(Test-Path -LiteralPath $archivePath)) { Invoke-WebRequest -Uri $url -OutFile $archivePath }
if ((Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash -ne $expected) { throw 'NVIDIA archive checksum mismatch. Existing plugin files were not modified.' }
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($archivePath)
try {
 foreach ($entry in $archive.Entries) {
  if ($entry.FullName -notmatch '^Plugins/(DLSS|Streamline|StreamlineCore|StreamlineDLSSG|StreamlineNGXCommon|StreamlineReflex)/' -or $entry.FullName.EndsWith('/')) { continue }
  $target=[IO.Path]::GetFullPath((Join-Path $projectRoot $entry.FullName))
  $pluginRoot=[IO.Path]::GetFullPath((Join-Path $projectRoot 'Plugins'))+'\'
  if (!$target.StartsWith($pluginRoot,[StringComparison]::OrdinalIgnoreCase)) { throw 'Invalid archive path.' }
  [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
  [IO.Compression.ZipFileExtensions]::ExtractToFile($entry,$target,$true)
 }
} finally { $archive.Dispose() }
Write-Output 'Official NVIDIA UE 5.8 plugins restored. No game build was packaged.'

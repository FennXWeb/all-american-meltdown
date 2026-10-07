$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$root = Split-Path -Parent $PSScriptRoot
function Resize-SMOGImage([string]$Source, [int]$Width, [int]$Height) {
    $src = [Drawing.Image]::FromFile($Source)
    $dst = [Drawing.Bitmap]::new($Width,$Height,[Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [Drawing.Graphics]::FromImage($dst)
    try {
        $g.Clear([Drawing.Color]::Transparent)
        $g.CompositingMode = [Drawing.Drawing2D.CompositingMode]::SourceCopy
        $g.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $g.PixelOffsetMode = [Drawing.Drawing2D.PixelOffsetMode]::HighQuality
        $g.DrawImage($src,[Drawing.Rectangle]::new(0,0,$Width,$Height))
    } finally { $g.Dispose(); $src.Dispose() }
    return $dst
}
foreach ($entry in @(@('header',2400,1000),@('logo',1200,400))) {
    $img = Resize-SMOGImage (Join-Path $root "ArtSource/SMOG84/$($entry[0])-source.png") $entry[1] $entry[2]
    try { $img.Save((Join-Path $root "smog_$($entry[0]).png"),[Drawing.Imaging.ImageFormat]::Png) } finally { $img.Dispose() }
}
# PNG-compressed ICO entries preserve the generated alpha channel at every size.
$sizes = @(16,32,48,256)
$frames = @()
foreach ($size in $sizes) {
    $img = Resize-SMOGImage (Join-Path $root 'ArtSource/SMOG84/icon-source.png') $size $size
    $stream = [IO.MemoryStream]::new()
    try { $img.Save($stream,[Drawing.Imaging.ImageFormat]::Png); $frames += ,$stream.ToArray() } finally { $stream.Dispose(); $img.Dispose() }
}
$file = [IO.File]::Create((Join-Path $root 'smog_icon.ico'))
$writer = [IO.BinaryWriter]::new($file)
try {
    $writer.Write([uint16]0); $writer.Write([uint16]1); $writer.Write([uint16]$sizes.Count)
    $offset = 6 + 16 * $sizes.Count
    for ($i=0; $i -lt $sizes.Count; $i++) {
        $encodedSize = if($sizes[$i] -eq 256){0}else{$sizes[$i]}
        $writer.Write([byte]$encodedSize); $writer.Write([byte]$encodedSize)
        $writer.Write([byte]0); $writer.Write([byte]0)
        $writer.Write([uint16]1); $writer.Write([uint16]32)
        $writer.Write([uint32]$frames[$i].Length); $writer.Write([uint32]$offset)
        $offset += $frames[$i].Length
    }
    foreach ($frame in $frames) { $writer.Write([byte[]]$frame) }
} finally { $writer.Dispose(); $file.Dispose() }
Write-Output 'SMOG artwork prepared: 2400x1000 header, 1200x400 alpha logo, 16/32/48/256 ICO.'

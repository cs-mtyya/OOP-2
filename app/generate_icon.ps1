$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$size = 256
$bitmap = [System.Drawing.Bitmap]::new($size, $size, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
$graphics.Clear([System.Drawing.Color]::FromArgb(24, 24, 24))

function New-RoundedPath([int]$x, [int]$y, [int]$width, [int]$height, [int]$radius) {
    $path = [System.Drawing.Drawing2D.GraphicsPath]::new()
    $diameter = $radius * 2
    $path.AddArc($x, $y, $diameter, $diameter, 180, 90)
    $path.AddArc($x + $width - $diameter, $y, $diameter, $diameter, 270, 90)
    $path.AddArc($x + $width - $diameter, $y + $height - $diameter, $diameter, $diameter, 0, 90)
    $path.AddArc($x, $y + $height - $diameter, $diameter, $diameter, 90, 90)
    $path.CloseFigure()
    return $path
}

$outer = New-RoundedPath 20 20 216 216 38
$backgroundBrush = [System.Drawing.SolidBrush]::new([System.Drawing.Color]::FromArgb(76, 76, 76))
$graphics.FillPath($backgroundBrush, $outer)

$pen = [System.Drawing.Pen]::new([System.Drawing.Color]::FromArgb(238, 238, 238), 12)
$frame = New-RoundedPath 48 44 160 168 20
$graphics.DrawPath($pen, $frame)
$mountain = [System.Drawing.Pen]::new([System.Drawing.Color]::FromArgb(190, 190, 190), 15)
$mountain.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
$mountain.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
$points = [System.Drawing.Point[]]@(
    [System.Drawing.Point]::new(65, 174),
    [System.Drawing.Point]::new(105, 128),
    [System.Drawing.Point]::new(133, 154),
    [System.Drawing.Point]::new(161, 118),
    [System.Drawing.Point]::new(193, 174)
)
$graphics.DrawLines($mountain, $points)
$sunBrush = [System.Drawing.SolidBrush]::new([System.Drawing.Color]::FromArgb(225, 225, 225))
$graphics.FillEllipse($sunBrush, 157, 70, 22, 22)

$stream = [System.IO.MemoryStream]::new()
$bitmap.Save($stream, [System.Drawing.Imaging.ImageFormat]::Png)
$png = $stream.ToArray()
$output = Join-Path $PSScriptRoot 'app_icon.ico'
$file = [System.IO.File]::Create($output)
$writer = [System.IO.BinaryWriter]::new($file)
$writer.Write([UInt16]0)
$writer.Write([UInt16]1)
$writer.Write([UInt16]1)
$writer.Write([Byte]0)
$writer.Write([Byte]0)
$writer.Write([Byte]0)
$writer.Write([Byte]0)
$writer.Write([UInt16]1)
$writer.Write([UInt16]32)
$writer.Write([UInt32]$png.Length)
$writer.Write([UInt32]22)
$writer.Write($png)
$writer.Dispose()
$file.Dispose()
$stream.Dispose()
$graphics.Dispose()
$bitmap.Dispose()
$outer.Dispose()
$frame.Dispose()
$backgroundBrush.Dispose()
$sunBrush.Dispose()
$pen.Dispose()
$mountain.Dispose()
Write-Host "Created $output"
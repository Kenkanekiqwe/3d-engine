$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$external = Join-Path $root "assets\external"
$level = Join-Path $root "assets\level"
$audio = Join-Path $root "assets\audio"

New-Item -ItemType Directory -Force $external,$level,$audio | Out-Null

# Real CC0 forest: 31 trees, 25 fence pieces, landscape, UVs and textures.
$forestUrl = "https://opengameart.org/sites/default/files/Low_Poly_Forest_Pack_Devilswork.Shop_v02.zip"
$forestZip = Join-Path $external "forest_pack.zip"
$forestDir = Join-Path $external "forest_pack"

if (!(Test-Path $forestDir)) {
    Write-Host "Downloading CC0 Low Poly Forest Pack..."
    Invoke-WebRequest -Uri $forestUrl -OutFile $forestZip
    Expand-Archive -Path $forestZip -DestinationPath $forestDir -Force
}

# Real CC0 house with OBJ and texture variants.
$houseUrl = "https://opengameart.org/sites/default/files/house_0.zip"
$houseZip = Join-Path $external "house_pack.zip"
$houseDir = Join-Path $external "house_pack"

if (!(Test-Path $houseDir)) {
    Write-Host "Downloading CC0 House..."
    Invoke-WebRequest -Uri $houseUrl -OutFile $houseZip
    Expand-Archive -Path $houseZip -DestinationPath $houseDir -Force
}

# Keep a predictable runtime location. The OBJ loader uses these files directly.
$houseObj = Get-ChildItem $houseDir -Recurse -Filter "house.obj" | Select-Object -First 1
if ($houseObj) { Copy-Item $houseObj.FullName (Join-Path $level "house.obj") -Force }

# Pick an actual tree mesh from the forest pack.
$treeObj = Get-ChildItem $forestDir -Recurse -Filter "*.obj" |
    Where-Object { $_.Name -match "tree|Tree|TREE" } |
    Select-Object -First 1
if ($treeObj) { Copy-Item $treeObj.FullName (Join-Path $level "tree.obj") -Force }

# Horror ambience remains separate from geometry.
$horrorUrl = "https://opengameart.org/sites/default/files/ambient_horror.wav"
$horrorFile = Join-Path $audio "ambient_horror.wav"
if (!(Test-Path $horrorFile)) {
    Write-Host "Downloading CC0 horror ambience..."
    Invoke-WebRequest -Uri $horrorUrl -OutFile $horrorFile
}

Write-Host ""
Write-Host "Real level assets installed:"
Write-Host "  assets\level\house.obj"
Write-Host "  assets\level\tree.obj"
Write-Host "  assets\audio\ambient_horror.wav"

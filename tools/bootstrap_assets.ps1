$ErrorActionPreference="Stop"
$root=Split-Path -Parent $PSScriptRoot
$external=Join-Path $root "assets\external";$level=Join-Path $root "assets\level";$trees=Join-Path $level "trees";$audio=Join-Path $root "assets\audio"
New-Item -ItemType Directory -Force $external,$level,$trees,$audio|Out-Null
function Get-Zip($url,$zip,$dir){if(!(Test-Path $dir)){Write-Host "Downloading $url";Invoke-WebRequest -Uri $url -OutFile $zip;Expand-Archive -Path $zip -DestinationPath $dir -Force}}
Get-Zip "https://opengameart.org/sites/default/files/Low_Poly_Forest_Pack_Devilswork.Shop_v02.zip" (Join-Path $external "forest.zip") (Join-Path $external "forest")
Get-Zip "https://opengameart.org/sites/default/files/ultimate_textured_building_pack_by_quaternius.zip" (Join-Path $external "buildings.zip") (Join-Path $external "buildings")
$forestDir=Join-Path $external "forest";$buildDir=Join-Path $external "buildings"
$objs=Get-ChildItem $forestDir -Recurse -Filter "*.obj"|Where-Object{$_.Name -match "tree|Tree|pine|Pine"}|Select-Object -First 8
$i=1;foreach($o in $objs){Copy-Item $o.FullName (Join-Path $trees ("tree$i.obj")) -Force; $mtl=[System.IO.Path]::ChangeExtension($o.FullName,".mtl");if(Test-Path $mtl){Copy-Item $mtl (Join-Path $trees ("tree$i.mtl")) -Force};$i++}
$house=Get-ChildItem $buildDir -Recurse -Filter "*.obj"|Where-Object{$_.Name -match "house|House|building|Building"}|Select-Object -First 1
if($house){Copy-Item $house.FullName (Join-Path $level "house.obj") -Force;$mtl=[System.IO.Path]::ChangeExtension($house.FullName,".mtl");if(Test-Path $mtl){Copy-Item $mtl (Join-Path $level "house.mtl") -Force}}
# Copy all nearby PNG/JPG assets so MTL-relative texture paths can be resolved.
Get-ChildItem $forestDir,$buildDir -Recurse -Include *.png,*.jpg,*.jpeg|ForEach-Object{Copy-Item $_.FullName (Join-Path $level $_.Name) -Force}
# Rewrite MTL texture paths to the runtime asset directory.
Get-ChildItem $trees,$level -Filter "*.mtl" -Recurse|ForEach-Object{$txt=Get-Content $_.FullName -Raw;$txt=[regex]::Replace($txt,'(?m)^map_Kd\s+(.+)$','map_Kd '+([System.IO.Path]::GetFileName($Matches[1])));Set-Content -Path $_.FullName -Value $txt -Encoding UTF8}
$horror=Join-Path $audio "ambient_horror.wav";if(!(Test-Path $horror)){Invoke-WebRequest -Uri "https://opengameart.org/sites/default/files/ambient_horror.wav" -OutFile $horror}
$rain=Join-Path $audio "rain.ogg";if(!(Test-Path $rain)){Invoke-WebRequest -Uri "https://opengameart.org/sites/default/files/Ove%20Melaa%20-%20Rainy%20%28NOT%20loopable%29%20Long%20Version.ogg" -OutFile $rain}
Write-Host "Assets installed: textured forest, building pack, quiet horror ambience and rain."

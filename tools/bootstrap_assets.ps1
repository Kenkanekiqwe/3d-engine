$ErrorActionPreference="Stop"
$root=Split-Path -Parent $PSScriptRoot
$external=Join-Path $root "assets\external";$level=Join-Path $root "assets\level";$trees=Join-Path $level "trees";$audio=Join-Path $root "assets\audio"
New-Item -ItemType Directory -Force $external,$level,$trees,$audio|Out-Null
function Get-Zip($url,$zip,$dir){if(!(Test-Path $dir)){Write-Host "Downloading $url";Invoke-WebRequest -Uri $url -OutFile $zip;Expand-Archive -Path $zip -DestinationPath $dir -Force}}
Get-Zip "https://opengameart.org/sites/default/files/Low_Poly_Forest_Pack_Devilswork.Shop_v02.zip" (Join-Path $external "forest.zip") (Join-Path $external "forest")
Get-Zip "https://opengameart.org/sites/default/files/ultimate_textured_building_pack_by_quaternius.zip" (Join-Path $external "buildings.zip") (Join-Path $external "buildings")
$forestDir=Join-Path $external "forest";$buildDir=Join-Path $external "buildings"
$objs=Get-ChildItem $forestDir -Recurse -Filter "*.obj"|Where-Object{$_.Name -match "tree|Tree|pine|Pine"}|Select-Object -First 8
$i=1
foreach($o in $objs){
 $dst=Join-Path $trees ("tree$i.obj");Copy-Item $o.FullName $dst -Force
 $mtl=[System.IO.Path]::ChangeExtension($o.FullName,".mtl")
 if(Test-Path $mtl){
   $mtlDst=Join-Path $trees ("tree$i.mtl");Copy-Item $mtl $mtlDst -Force
   $objTxt=Get-Content $dst -Raw
   $objTxt=[regex]::Replace($objTxt,'(?m)^mtllib\s+.*$','mtllib tree'+$i+'.mtl')
   Set-Content -Path $dst -Value $objTxt -Encoding UTF8
   $mtxt=Get-Content $mtlDst -Raw
   $mtxt=[regex]::Replace($mtxt,'(?m)^map_Kd\s+(.+)$',{param($m) "map_Kd "+[System.IO.Path]::GetFileName($m.Groups[1].Value.Trim())})
   Set-Content -Path $mtlDst -Value $mtxt -Encoding UTF8
   Get-ChildItem (Split-Path $mtl -Parent) -Recurse -File -Include *.png,*.jpg,*.jpeg|ForEach-Object{
      if($mtxt -match [regex]::Escape($_.Name)){Copy-Item $_.FullName (Join-Path $trees $_.Name) -Force}
   }
 }
 $i++
}
$house=Get-ChildItem $buildDir -Recurse -Filter "*.obj"|Where-Object{$_.Name -match "house|House|building|Building"}|Select-Object -First 1
if($house){
 $houseDst=Join-Path $level "house.obj";Copy-Item $house.FullName $houseDst -Force
 $mtl=[System.IO.Path]::ChangeExtension($house.FullName,".mtl")
 if(Test-Path $mtl){
   Copy-Item $mtl (Join-Path $level "house.mtl") -Force
   $objTxt=Get-Content $houseDst -Raw;$objTxt=[regex]::Replace($objTxt,'(?m)^mtllib\s+.*$','mtllib house.mtl');Set-Content -Path $houseDst -Value $objTxt -Encoding UTF8
   $mtxt=Get-Content (Join-Path $level "house.mtl") -Raw
   $mtxt=[regex]::Replace($mtxt,'(?m)^map_Kd\s+(.+)$',{param($m) "map_Kd "+[System.IO.Path]::GetFileName($m.Groups[1].Value.Trim())})
   Set-Content -Path (Join-Path $level "house.mtl") -Value $mtxt -Encoding UTF8
   Get-ChildItem (Split-Path $mtl -Parent) -Recurse -File -Include *.png,*.jpg,*.jpeg|ForEach-Object{
      if($mtxt -match [regex]::Escape($_.Name)){Copy-Item $_.FullName (Join-Path $level $_.Name) -Force}
   }
 }
}
$horror=Join-Path $audio "ambient_horror.wav";if(!(Test-Path $horror)){Invoke-WebRequest -Uri "https://opengameart.org/sites/default/files/ambient_horror.wav" -OutFile $horror}
Write-Host "Assets installed: textured forest, textured building, and quiet horror ambience."

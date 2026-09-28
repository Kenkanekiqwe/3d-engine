$ErrorActionPreference="Stop"
$root=Split-Path -Parent $PSScriptRoot
$level=Join-Path $root "assets\level";$trees=Join-Path $level "trees";$audio=Join-Path $root "assets\audio";$external=Join-Path $root "assets\external"
New-Item -ItemType Directory -Force $level,$trees,$audio,$external|Out-Null
function Download-Poly($id,$name){
 $headers=@{"User-Agent"="CryHorrorEngine/1.0"}
 $files=Invoke-RestMethod -Headers $headers -Uri ("https://api.polyhaven.com/files/"+$id)
 $model=$files.model
 if($model){
  foreach($res in @("1k","2k")){
   if($model.$res){
    foreach($fmt in @("gltf","obj")){
     if($model.$res.$fmt){$url=$model.$res.$fmt.url; if($url){$out=Join-Path $external ($name+"_"+$fmt+".zip");Invoke-WebRequest -Headers $headers -Uri $url -OutFile $out;return $out}}
    }
   }
  }
 }
 return $null
}
# Real, CC0 Poly Haven source models. These replace the old low-poly forest.
$treeIds=@("pine_tree_01","fir_tree_01","pine_sapling_medium")
$i=1
foreach($id in $treeIds){
 try{ $zip=Download-Poly $id ("tree"+$i); if($zip -and $zip.EndsWith(".zip")){Expand-Archive $zip -DestinationPath (Join-Path $external ("tree"+$i)) -Force; $obj=Get-ChildItem (Join-Path $external ("tree"+$i)) -Recurse -Filter "*.obj"|Select-Object -First 1;if($obj){Copy-Item $obj.FullName (Join-Path $trees ("tree"+$i+".obj")) -Force;$mtl=[IO.Path]::ChangeExtension($obj.FullName,".mtl");if(Test-Path $mtl){Copy-Item $mtl (Join-Path $trees ("tree"+$i+".mtl")) -Force}}} }catch{Write-Warning ("Poly Haven "+$id+" failed: "+$_.Exception.Message)};$i++
}
# Do not install the old fantasy/low-poly house. Keep a clear slot for the photogrammetry house.
Write-Host "Forest source: Poly Haven CC0 Pine/Fir assets."
Write-Host "House source selected for replacement: Forest House Ruin photogrammetry, CC BY 4.0."
Write-Host "Download the house from Fab/Sketchfab and place its converted OBJ+MTL+textures at assets\level\house.obj."
$horror=Join-Path $audio "ambient_horror.wav";if(!(Test-Path $horror)){Invoke-WebRequest -Uri "https://opengameart.org/sites/default/files/ambient_horror.wav" -OutFile $horror}

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$external = Join-Path $root "assets\external"
$audio = Join-Path $root "assets\audio"
New-Item -ItemType Directory -Force $external | Out-Null
New-Item -ItemType Directory -Force $audio | Out-Null

# Kenney Nature Kit 2.1 — CC0
$kenneyUrl = "https://kenney.nl/media/pages/assets/nature-kit/37ac38a37b-1677698939/kenney_nature-kit.zip"
$kenneyZip = Join-Path $external "kenney_nature-kit.zip"
Write-Host "Downloading Kenney Nature Kit..."
Invoke-WebRequest -Uri $kenneyUrl -OutFile $kenneyZip
Expand-Archive -Path $kenneyZip -DestinationPath (Join-Path $external "kenney_nature-kit") -Force

# OpenGameArt Ambient Horror — CC0
$horrorUrl = "https://opengameart.org/sites/default/files/ambient_horror.wav"
$horrorFile = Join-Path $audio "ambient_horror.wav"
Write-Host "Downloading CC0 horror ambience..."
Invoke-WebRequest -Uri $horrorUrl -OutFile $horrorFile

Write-Host ""
Write-Host "Assets installed."
Write-Host "Environment: assets\external\kenney_nature-kit"
Write-Host "Audio:       assets\audio\ambient_horror.wav"

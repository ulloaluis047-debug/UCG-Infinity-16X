$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root "build"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
  winget install --id Kitware.CMake --exact --accept-source-agreements --accept-package-agreements
  $env:Path += ";C:\Program Files\CMake\bin"
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
  winget install --id Git.Git --exact --accept-source-agreements --accept-package-agreements
}

cmake -S $root -B $build -G "Visual Studio 17 2022" -A x64
cmake --build $build --config Release --target UCGInfinity16X_VST3 UCGInfinity16X_Standalone

$vst = Get-ChildItem $build -Recurse -Filter "UCG Infinity 16X.vst3" | Select-Object -First 1
Write-Host "COMPILACIÓN COMPLETA" -ForegroundColor Green
Write-Host $vst.FullName

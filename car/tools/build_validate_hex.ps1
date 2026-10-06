param([switch]$Clean)
$ErrorActionPreference = "Stop"
$workspace = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$ccsRoot = $env:CCS_INSTALL_DIR
if ([string]::IsNullOrWhiteSpace($ccsRoot)) { throw "Set CCS_INSTALL_DIR to the CCS installation directory" }
$ccsGmake = Join-Path $ccsRoot "ccs\utils\bin\gmake.exe"
$compilerRoot = Get-ChildItem (Join-Path $ccsRoot "ccs\tools\compiler") -Directory -Filter "ti-cgt-armllvm_*" |
    Sort-Object Name -Descending | Select-Object -First 1
if ($null -eq $compilerRoot) { throw "TI ARM Clang compiler not found under CCS_INSTALL_DIR" }
$hexTool = Join-Path $compilerRoot.FullName "bin\tiarmhex.exe"
$projectDir = Join-Path $workspace "car"
$debugDir = Join-Path $projectDir "Debug"
$hexPath = Join-Path $projectDir "car.hex"
$validator = Join-Path $workspace "skills\mspm0-ccs\scripts\validate_hex.py"
if (-not (Test-Path $ccsGmake)) { throw "CCS gmake not found: $ccsGmake" }
if (-not (Test-Path $hexTool)) { throw "TI ARM Hex tool not found: $hexTool" }
if ($Clean) { & $ccsGmake -C $debugDir clean all } else { & $ccsGmake -C $debugDir all }
if ($LASTEXITCODE -ne 0) { throw "CCS build failed" }
& $hexTool --intel --memwidth=8 --romwidth=8 --outfile=$hexPath (Join-Path $debugDir "car.out")
if ($LASTEXITCODE -ne 0) { throw "HEX generation failed" }
python $validator $hexPath
if ($LASTEXITCODE -ne 0) { throw "HEX validation failed; refusing to publish the image" }
Write-Host "Validated flash image: $hexPath"

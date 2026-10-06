param([switch]$Clean)
$ErrorActionPreference = "Stop"
$workspace = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$ccsGmake = "D:\TI\CCS\ccs\utils\bin\gmake.exe"
$hexTool = "D:\TI\CCS\ccs\tools\compiler\ti-cgt-armllvm_5.1.1.LTS\bin\tiarmhex.exe"
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

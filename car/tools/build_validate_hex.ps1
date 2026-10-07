[CmdletBinding()]
param(
    [string]$CcsInstallDir = $env:CCS_INSTALL_DIR,
    [string]$ProjectDir = "",
    [string]$OutputHex = "",
    [string]$FirmwareAlias = "",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

function Stop-Build([string]$Message) {
    throw "HEX build stopped: $Message"
}

$scriptProjectDir = if ([string]::IsNullOrWhiteSpace($ProjectDir)) {
    Join-Path $PSScriptRoot ".."
} else {
    $ProjectDir
}
$project = (Resolve-Path -LiteralPath $scriptProjectDir).Path
$workspace = (Resolve-Path (Join-Path $project "..")).Path
$debugDir = Join-Path $project "Debug"
$output = if ([string]::IsNullOrWhiteSpace($OutputHex)) {
    Join-Path $project "car.hex"
} else {
    [IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputHex))
}
$validator = Join-Path $workspace "skills\mspm0-ccs\scripts\validate_hex.py"

if ([string]::IsNullOrWhiteSpace($CcsInstallDir)) {
    Stop-Build "CCS_INSTALL_DIR is not set. Set it to the CCS installation directory, for example: `$env:CCS_INSTALL_DIR = '<CCS install directory>'"
}

$ccsRoot = (Resolve-Path -LiteralPath $CcsInstallDir).Path
$gmake = Join-Path $ccsRoot "ccs\utils\bin\gmake.exe"
$compilerRoot = Get-ChildItem (Join-Path $ccsRoot "ccs\tools\compiler") -Directory -Filter "ti-cgt-armllvm_*" |
    Sort-Object Name -Descending | Select-Object -First 1

if (-not (Test-Path -LiteralPath $gmake)) { Stop-Build "CCS gmake not found: $gmake" }
if ($null -eq $compilerRoot) { Stop-Build "TI Arm Clang compiler not found under $ccsRoot" }
$hexTool = Join-Path $compilerRoot.FullName "bin\tiarmhex.exe"
if (-not (Test-Path -LiteralPath $hexTool)) { Stop-Build "TI HEX tool not found: $hexTool" }
if (-not (Test-Path -LiteralPath $validator)) { Stop-Build "HEX validator not found: $validator" }
if (-not (Test-Path -LiteralPath (Join-Path $debugDir "subdir_rules.mk"))) {
    Stop-Build "This directory is not a CCS Debug build directory: $debugDir"
}

Write-Host "[1/4] Build project: $project"
if ($Clean) { & $gmake -C $debugDir clean all } else { & $gmake -C $debugDir all }
if ($LASTEXITCODE -ne 0) { Stop-Build "CCS compilation/linking failed" }

$outFile = Join-Path $debugDir "car.out"
if (-not (Test-Path -LiteralPath $outFile)) { Stop-Build "Build did not produce $outFile" }

Write-Host "[2/4] Generate Intel HEX: $output"
$outputParent = Split-Path -Parent $output
New-Item -ItemType Directory -Force -Path $outputParent | Out-Null
& $hexTool --intel --memwidth=8 --romwidth=8 --outfile=$output $outFile
if ($LASTEXITCODE -ne 0) { Stop-Build "Intel HEX generation failed" }

Write-Host "[3/4] Validate Intel HEX checksum and MSPM0 BSL alignment"
python $validator $output
if ($LASTEXITCODE -ne 0) { Stop-Build "HEX validation failed; no image was published" }

if (-not [string]::IsNullOrWhiteSpace($FirmwareAlias)) {
    $alias = [IO.Path]::GetFullPath((Join-Path (Get-Location) $FirmwareAlias))
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $alias) | Out-Null
    Copy-Item -LiteralPath $output -Destination $alias -Force
    python $validator $alias
    if ($LASTEXITCODE -ne 0) { Stop-Build "Firmware alias validation failed: $alias" }
    Write-Host "Alias updated: $alias"
}

Write-Host "[4/4] SHA256"
Get-FileHash -LiteralPath $output -Algorithm SHA256 | Format-Table -AutoSize
Write-Host "Validated flash image: $output"

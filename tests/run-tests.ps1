# Builds and runs the host (PC) unit tests with MSVC; optionally compiles the firmware too.
# Usage:  .\tests\run-tests.ps1            # host tests only
#         .\tests\run-tests.ps1 -Firmware  # host tests + CoreInk firmware compile
param([switch]$Firmware)

$ErrorActionPreference = 'Stop'
$tests = $PSScriptRoot
$src = Join-Path (Split-Path $tests) 'blackjack'
$kit = Join-Path $src 'src\CoreInkKit'
$stubs = Join-Path $tests 'stubs'
$out = Join-Path $env:TEMP 'bj-host-tests'
New-Item -ItemType Directory -Force $out | Out-Null

# Test program -> production units it links against (Blackjack or CoreInkKit; found by name).
$suites = [ordered]@{
    'round_judge'      = 'RoundJudge', 'Hand', 'Card', 'Score'
    'round_controller' = 'RoundController', 'RoundJudge', 'Hand', 'Card', 'Score'
    'deck_shuffle'     = 'Deck', 'Card'
    'battery_monitor'  = 'BatteryMonitor'
    'table_screen'     = 'TableScreen', 'HandView', 'CardRenderer', 'PromptBar', 'Buttons',
                         'LightSleep', 'BatteryMonitor', 'Hand', 'Card', 'Score'
    'screens'          = 'SplashScreen', 'MenuScreen', 'PagedTextScreen', 'MessageScreen',
                         'CardRenderer', 'PromptBar', 'Buttons', 'Card'
    'app_states'       = 'BlackjackApp', 'SplashScreen', 'MenuScreen', 'PagedTextScreen',
                         'MessageScreen', 'TableScreen', 'HandView', 'CardRenderer', 'PromptBar',
                         'Buttons', 'LightSleep', 'BatteryMonitor', 'RoundController',
                         'RoundJudge', 'Deck', 'Hand', 'Card', 'Score'
}

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) { throw 'MSVC build tools not found (install Visual Studio Build Tools with the C++ workload).' }
$vcvars = Join-Path $vsPath 'VC\Auxiliary\Build\vcvars64.bat'

$failed = @()

# CoreInkKit must stay independent of the Blackjack code: its quoted includes may only name kit files.
foreach ($file in Get-ChildItem $kit -File) {
    foreach ($match in Select-String -Path $file.FullName -Pattern '#include "([^"]+)"') {
        $included = $match.Matches[0].Groups[1].Value
        if (-not (Test-Path (Join-Path $kit $included))) {
            Write-Host "CoreInkKit\$($file.Name) includes non-kit file '$included'" -ForegroundColor Red
            $failed += 'kit independence'
        }
    }
}
foreach ($name in $suites.Keys) {
    $sources = @("`"$tests\$name.cpp`"") + ($suites[$name] | ForEach-Object {
        $file = @("$src\$_.cpp", "$kit\$_.cpp") | Where-Object { Test-Path $_ } | Select-Object -First 1
        if (-not $file) { throw "Suite '$name': no source for unit '$_'." }
        "`"$file`""
    })
    $exe = Join-Path $out "$name.exe"
    $cl = "cl /nologo /std:c++17 /EHsc /utf-8 /W4 /I`"$stubs`" /Fo`"$out\\`" /Fe`"$exe`" $($sources -join ' ')"
    Write-Host "== $name" -ForegroundColor Cyan
    & $env:ComSpec /c "call `"$vcvars`" >nul && $cl >`"$out\$name.log`" 2>&1"
    if ($LASTEXITCODE -ne 0) {
        Get-Content "$out\$name.log" | Where-Object { $_ -match 'error|warning' } | Write-Host
        $failed += "$name (build)"
        continue
    }
    Get-Content "$out\$name.log" | Where-Object { $_ -match 'warning' } | Write-Host -ForegroundColor Yellow
    & $exe
    if ($LASTEXITCODE -ne 0) { $failed += $name }
}

if ($Firmware) {
    Write-Host '== firmware' -ForegroundColor Cyan
    $cli = "$env:LOCALAPPDATA\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
    # A short build path avoids Windows path-length failures inside M5Unified.
    & $cli compile --build-path "$env:TEMP\bj-fw" --fqbn m5stack:esp32:m5stack_coreink $src 2>&1 |
        Select-String 'error|Sketch uses|Global variables' | ForEach-Object Line
    if ($LASTEXITCODE -ne 0) { $failed += 'firmware' }
}

if ($failed) {
    Write-Host "FAILED: $($failed -join ', ')" -ForegroundColor Red
    exit 1
}
Write-Host 'All tests passed.' -ForegroundColor Green

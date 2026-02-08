# RTI Ping-Pong 실행 스크립트
# run.ps1

$releaseDir = "build\Release"

# Check if executables exist
if (-Not (Test-Path "$releaseDir\PingDaemon.exe")) {
    Write-Host "[Error] PingDaemon.exe not found. Run './build.ps1' first." -ForegroundColor Red
    exit 1
}

if (-Not (Test-Path "$releaseDir\PingGui.exe")) {
    Write-Host "[Error] PingGui.exe not found. Run './build.ps1' first." -ForegroundColor Red
    exit 1
}

Write-Host "--- Starting RTI Ping-Pong Applications ---" -ForegroundColor Cyan

# Start Daemon (터미널 창에서 실행)
Write-Host "[1/2] Starting PingDaemon..." -ForegroundColor Yellow
Start-Process -FilePath "$releaseDir\PingDaemon.exe" -WorkingDirectory $releaseDir

# Wait a moment for daemon to initialize
Start-Sleep -Milliseconds 500

# Start GUI
Write-Host "[2/2] Starting PingGui..." -ForegroundColor Yellow
Start-Process -FilePath "$releaseDir\PingGui.exe" -WorkingDirectory $releaseDir

Write-Host "--- Both applications started ---" -ForegroundColor Green
Write-Host "Close PingDaemon window to test disconnect detection." -ForegroundColor Cyan

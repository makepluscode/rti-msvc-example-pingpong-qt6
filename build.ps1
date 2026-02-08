# RTI Ping-Pong 빌드 스크립트
# build.ps1

Write-Host "--- [Step 1] Initializing Build Directory ---" -ForegroundColor Cyan
if (-Not (Test-Path "build")) { 
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location build

Write-Host "--- [Step 2] Configuring with CMake ---" -ForegroundColor Cyan
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.10.2/msvc2022_64" -Wno-dev

Write-Host "--- [Step 3] Building Projects ---" -ForegroundColor Cyan
cmake --build . --config Release

Set-Location ..

# Copy license and QoS files
Write-Host "--- [Step 4] Copying Runtime Files ---" -ForegroundColor Cyan
Copy-Item "C:\Program Files\rti_connext_dds-7.5.0\rti_license.dat" -Destination "build\Release\" -Force -ErrorAction SilentlyContinue
Copy-Item "USER_QOS_PROFILES.xml" -Destination "build\Release\" -Force -ErrorAction SilentlyContinue

Write-Host "--- [Build Complete] ---" -ForegroundColor Green
Write-Host "Run './run.ps1' to start both applications." -ForegroundColor Yellow

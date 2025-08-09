@echo off
REM Enhanced Windows Docker run script for 3D Black Hole Simulation
REM Requires VcXsrv or Xming for X11 support

echo 🚀 Starting 3D Black Hole Simulation on Windows
echo ===============================================

REM Check if Docker is available
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Docker is not installed or not in PATH
    echo Please install Docker Desktop and try again
    pause
    exit /b 1
)

REM Check if Docker is running
docker info >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Docker is not running. Please start Docker Desktop and try again.
    pause
    exit /b 1
)

echo 📦 Building Docker image...
docker build -t black-hole-3d .

if %errorlevel% neq 0 (
    echo ❌ Docker build failed!
    pause
    exit /b 1
)

echo 🎮 Starting simulation container...
echo 📺 Connect to VNC at localhost:5900 to view the simulation
echo 🛑 Press Ctrl+C to stop the container

REM Run container
docker run -it --rm ^
    --name black-hole-sim ^
    -p 5900:5900 ^
    -v "%cd%\\config:/app/config" ^
    -v "%cd%\\logs:/app/logs" ^
    black-hole-3d

echo ✅ Simulation stopped.
pause
